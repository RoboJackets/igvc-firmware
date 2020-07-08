#include "mbed.h"

#include <cstring>
#include <string>

#include <EthernetInterface.h>
#include <pb_decode.h>
#include <pb_encode.h>
#include "igvc.pb.h"
#include "sabertooth_controller/sabertooth_controller.h"
#include "utils.h"
#include "pid.h"

/* hardware definitions */
SaberToothController g_motor_controller;

/* mbed pin definitions */
DigitalOut g_mbed_led1(LED1);
DigitalOut g_mbed_led2(LED2);
DigitalOut g_mbed_led3(LED3);
DigitalOut g_mbed_led4(LED4);
DigitalOut g_board_led(p8);
DigitalOut g_safety_light_enable(p11);
DigitalIn g_e_stop_status(p15);
AnalogIn g_battery(p19);

/* PID calculation values */
float g_i_error_l = 0;
float g_i_error_r = 0;
float g_d_t_sec = 0;

/* Motor Data (see utils.h) */
MotorCoeffs g_motor_coeffs;
MotorStatusPair g_motor_pair;

/* e-stop logic */
int g_estop = 1;

/* function prototypes */
void parseRequest(const RequestMessage &req);
bool sendResponse(TCPSocket &client);
void triggerEstop();

/* PID Control Thread Variables */
Mutex g_pid_lock;
Thread g_control_thread;
PIDArgs g_pid_io{&g_motor_controller, &g_d_t_sec, &g_i_error_l, &g_i_error_r, &g_motor_coeffs, &g_motor_pair};

int main()
{
  g_control_thread.set_priority(osPriorityHigh);
  g_control_thread.start(callback(pid_thread, &g_pid_io));

  Serial pc(USBTX, USBRX); // For debugging
  pc.printf("Connecting...\r\n");

  /* Open the server (mbed) via the EthernetInterface class */
  EthernetInterface net;

  if (int ret = net.set_network(MBED_IP, NETMASK, COMPUTER_IP); ret != 0)
  {
    pc.printf("Error performing set_network(). Error code: %i\r\n", ret);
    return 1;
  }
  if (int ret = net.connect(); ret != 0)
  {
    pc.printf("Error performing connect(). Error code: %i\r\n", ret);
    return 1;
  }

  const char *ip = net.get_ip_address();
  pc.printf("MBED's IP address is: %s\n", ip ? ip : "No IP");

  /* Instantiate a TCP Socket to function as the server and bind it to the specified port */
  TCPSocket server_socket;
  if (int ret = server_socket.open(&net); ret != 0)
  {
    pc.printf("Error opening TCPSocket. Error code: %i\r\n", ret);
    return 1;
  }

  if (int ret = server_socket.bind(MBED_IP, SERVER_PORT); ret != 0)
  {
    pc.printf("Error binding TCPSocket. Error code: %i\r\n", ret);
    return 1;
  }

  if (int ret = server_socket.listen(1); ret != 0)
  {
    pc.printf("Error listening. Error code: %i\r\n", ret);
    return 1;
  }

  /* Instantiage a TCP socket to serve as the client */
  TCPSocket *client = nullptr;

  while (true)
  {
    g_mbed_led2 = 1;
    /* wait for a new TCP Connection */
    pc.printf("Waiting for new connection...\r\n");
    client = server_socket.accept();
    g_mbed_led2 = 0;

    SocketAddress socket_address;
    client->getpeername(&socket_address);
    pc.printf("Accepted client from %s\r\n", socket_address.get_ip_address());

    g_estop = 1;

    while (true)
    {
      /* read data into the buffer. This call blocks until data is read */
      char buffer[BUFFER_SIZE];
      int n = client->recv(buffer, sizeof(buffer) - 1);

      /*
      n represents the response message for the read() command.
      - if n == 0 then the client closed the connection
      - otherwise, n is the number of bytes read
      */
      if (n < 0)
      {
        if (DEBUG)
        {
          printf("Received empty buffer\n");
        }
        wait_ms(10);
        continue;
      }
      if (n == 0)
      {
        pc.printf("Client Closed Connection\n");
        break;
      }
      if (DEBUG)
      {
        printf("Received Request of size: %d\n", n);
      }

      /* protobuf message to hold request from client */
      /* Create a stream that reads from the buffer. */
      RequestMessage request = RequestMessage_init_zero;
      pb_istream_t istream = pb_istream_from_buffer(reinterpret_cast<uint8_t *>(buffer), n);

      /* check for any errors.. */
      if (!pb_decode(&istream, RequestMessage_fields, &request))
      {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&istream));
        continue;
      }

      parseRequest(request);

      /* estop logic */
      if (g_e_stop_status.read() == 0)
      {
        triggerEstop();
      }
      else
      {
        g_estop = 1;
        g_safety_light_enable = 0;
      }

      if (!sendResponse(*client))
      {
        printf("Couldn't send response to client!\r\n");
        continue;
      }
    }
    pc.printf("Closing rip..\r\n");
    triggerEstop();
    client->close();
  }
}

bool sendResponse(TCPSocket &client)
{
  /* protocol buffer to hold response message */
  ResponseMessage response = ResponseMessage_init_zero;

  /* This is the buffer where we will store the response message. */
  uint8_t responsebuffer[256];
  size_t response_length;
  bool ostatus;

  /* Create a stream that will write to our buffer. */
  pb_ostream_t ostream = pb_ostream_from_buffer(responsebuffer, sizeof(responsebuffer));

  /* Fill in the message fields */
  response.has_p_l = true;
  response.has_p_r = true;
  response.has_i_l = true;
  response.has_i_r = true;
  response.has_d_l = true;
  response.has_d_r = true;
  response.has_speed_l = true;
  response.has_speed_r = true;
  response.has_dt_sec = true;
  response.has_voltage = true;
  response.has_estop = true;

  response.has_kv_l = true;
  response.has_kv_r = true;

  response.has_left_output = true;
  response.has_right_output = true;

  g_pid_lock.lock();
  response.p_l = static_cast<float>(g_motor_coeffs.left.k_p);
  response.p_r = static_cast<float>(g_motor_coeffs.right.k_p);
  response.i_l = static_cast<float>(g_motor_coeffs.left.k_i);
  response.i_r = static_cast<float>(g_motor_coeffs.right.k_i);
  response.d_l = static_cast<float>(g_motor_coeffs.left.k_d);
  response.d_r = static_cast<float>(g_motor_coeffs.right.k_d);
  response.speed_l = static_cast<float>(g_motor_pair.left.actual_speed);
  response.speed_r = static_cast<float>(g_motor_pair.right.actual_speed);
  response.dt_sec = static_cast<float>(g_d_t_sec);
  response.kv_l = static_cast<float>(g_motor_coeffs.left.k_kv);
  response.kv_r = static_cast<float>(g_motor_coeffs.right.k_kv);
  response.left_output = g_motor_pair.left.ctrl_output;
  response.right_output = g_motor_pair.right.ctrl_output;
  g_pid_lock.unlock();

  response.voltage = static_cast<float>(g_battery.read() * 3.3 * 521 / 51);
  response.estop = static_cast<bool>(g_estop);

  /* encode the message */
  ostatus = pb_encode(&ostream, ResponseMessage_fields, &response);
  response_length = ostream.bytes_written;

  if (DEBUG)
  {
    printf("Sending message of length: %zu\n", response_length);
  }

  /* Then just check for any errors.. */
  if (!ostatus)
  {
    printf("Encoding failed: %s\n", PB_GET_ERROR(&ostream));
    return false;
  }

  client.send(reinterpret_cast<char *>(responsebuffer), response_length);
  return true;
}

void triggerEstop()
{
  // If get 5V, since inverted, meaning disabled on motors
  g_estop = 0;
  g_safety_light_enable = 1;
  g_pid_lock.lock();
  g_motor_pair.left.desired_speed = 0;
  g_motor_pair.right.desired_speed = 0;
  g_i_error_l = 0;
  g_i_error_r = 0;
  g_motor_controller.stopMotors();
  g_pid_lock.unlock();
}

/*
Update global variables using most recent client request.
@param[in] req RequestMessage protobuf with desired values
*/
void parseRequest(const RequestMessage &req)
{
  /* request contains PID values */
  if (req.has_p_l)
  {
    g_pid_lock.lock();
    g_motor_coeffs.left.k_p = req.p_l;
    g_motor_coeffs.right.k_p = req.p_r;
    g_motor_coeffs.left.k_d = req.d_l;
    g_motor_coeffs.right.k_d = req.d_r;
    g_motor_coeffs.left.k_i = req.i_l;
    g_motor_coeffs.right.k_i = req.i_r;
    g_motor_coeffs.left.k_kv = req.kv_l;
    g_motor_coeffs.right.k_kv = req.kv_r;
    g_pid_lock.unlock();
  }
  /* request contains motor velocities */
  if (req.has_speed_l)
  {
    g_pid_lock.lock();
    g_motor_pair.left.desired_speed = req.speed_l;
    g_motor_pair.right.desired_speed = req.speed_r;
    g_pid_lock.unlock();
  }
}
