#include "mbed.h"

#include <cstring>
#include <string>

#include <EthernetInterface.h>
#include <pb_decode.h>
#include <pb_encode.h>
#include "igvc.pb.h"
#include "encoder_pair/encoder_pair.h"
#include "sabertooth_controller/sabertooth_controller.h"
#include "constants.h"
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

/* desired motor speed (as specified by the client) */
float g_desired_speed_l = 0;
float g_desired_speed_r = 0;

/* actual motor speeds */
float g_actual_speed_l = 0;
float g_actual_speed_r = 0;

/* PID calculation values */
float g_i_error_l = 0;
float g_i_error_r = 0;
float g_d_t_sec = 0;

/* PID constants */
float g_p_l = 0;
float g_d_l = 0;
float g_p_r = 0;
float g_d_r = 0;
float g_i_l = 0;
float g_i_r = 0;
float g_kv_l = 0;
float g_kv_r = 0;

/* motor outputs */
uint32_t g_left_output;
uint32_t g_right_output;

/* e-stop logic */
int g_estop = 1;

/* function prototypes */
void parseRequest(const RequestMessage &req);
bool sendResponse(TCPSocket &client);
void triggerEstop();

Thread control_thread;

int main()
{
  control_thread.start(pid_thread);

  //    /* Read PCON register */
  //  printf("PCON: 0x%x\n", *((unsigned int *)0x400FC180));
  //  *(unsigned int *)0x400fc180 |= 0xf;

  Serial pc(USBTX, USBRX);
  /* Open the server (mbed) via the EthernetInterface class */
  pc.printf("Connecting...\r\n");
  EthernetInterface net;

  if (int ret = net.set_network(G_MBED_IP, G_NETMASK, G_COMPUTER_IP); ret != 0)
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

  /* Instantiate a TCP Socket to function as the server and bind it to the
   * specified port */
  TCPSocket server_socket;
  if (int ret = server_socket.open(&net); ret != 0)
  {
    pc.printf("Error opening TCPSocket. Error code: %i\r\n", ret);
    return 1;
  }

  if (int ret = server_socket.bind(G_MBED_IP, G_SERVER_PORT); ret != 0)
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
      char buffer[G_BUFFER_SIZE];
      int n = client->recv(buffer, sizeof(buffer) - 1);

      /*
      n represents the response message for the read() command.
      - if n == 0 then the client closed the connection
      - otherwise, n is the number of bytes read
      */
      if (n < 0)
      {
        if (G_DEBUG)
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
      if (G_DEBUG)
      {
        printf("Received Request of size: %d\n", n);
      }


      /* protobuf message to hold request from client */
      RequestMessage request = RequestMessage_init_zero;
      bool istatus;

      /* Create a stream that reads from the buffer. */
      pb_istream_t istream = pb_istream_from_buffer(reinterpret_cast<uint8_t *>(buffer), n);

      /* decode the message */
      istatus = pb_decode(&istream, RequestMessage_fields, &request);

      /* check for any errors.. */
      if (!istatus)
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

  response.p_l = static_cast<float>(g_p_l);
  response.p_r = static_cast<float>(g_p_r);
  response.i_l = static_cast<float>(g_i_l);
  response.i_r = static_cast<float>(g_i_r);
  response.d_l = static_cast<float>(g_d_l);
  response.d_r = static_cast<float>(g_d_r);

  response.speed_l = static_cast<float>(g_actual_speed_l);
  response.speed_r = static_cast<float>(g_actual_speed_r);
  response.dt_sec = static_cast<float>(g_d_t_sec);
  response.voltage = static_cast<float>(g_battery.read() * 3.3 * 521 / 51);
  response.estop = static_cast<bool>(g_estop);

  response.kv_l = static_cast<float>(g_kv_l);
  response.kv_r = static_cast<float>(g_kv_r);

  response.left_output = g_left_output;
  response.right_output = g_right_output;

  /* encode the message */
  ostatus = pb_encode(&ostream, ResponseMessage_fields, &response);
  response_length = ostream.bytes_written;

  if (G_DEBUG)
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
  g_desired_speed_l = 0;
  g_desired_speed_r = 0;
  g_i_error_l = 0;
  g_i_error_r = 0;
  g_motor_controller.stopMotors();
  g_safety_light_enable = 1;
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
    g_p_l = req.p_l;
    g_p_r = req.p_r;
    g_d_l = req.d_l;
    g_d_r = req.d_r;
    g_i_l = req.i_l;
    g_i_r = req.i_r;
    g_kv_l = req.kv_l;
    g_kv_r = req.kv_r;
  }
  /* request contains motor velocities */
  if (req.has_speed_l)
  {
    g_desired_speed_l = req.speed_l;
    g_desired_speed_r = req.speed_r;
  }
}

