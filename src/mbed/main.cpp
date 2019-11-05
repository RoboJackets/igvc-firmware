#include "mbed.h"

#include <cstring>
#include <cstdlib>
#include <string>

#define DEBUG false

#include "igvc.pb.h"
#include <EthernetInterface.h>
#include <pb_decode.h>
#include <pb_encode.h>

/* ethernet setup variables */
#define ECHO_SERVER_PORT 7
#define BUFFER_SIZE 256
#define MAX_MESSAGES 1 // backlog of messages the server should hold
#define TIMEOUT_MS 50  // timeout for blocking read operations

/* hardware definitions */
Timer g_timer;
Serial g_serial(p13, NC, 9600);

/* mbed pin definitions */
DigitalOut g_my_led1(LED1);
DigitalOut g_my_led2(LED2);
DigitalOut g_my_le_d3(LED3);
DigitalOut g_my_le_d4(LED4);
DigitalOut g_board_led(p8);
DigitalOut g_e_stop_light(p11);
DigitalIn g_e_stop_status(p15);

InterruptIn g_encoder_left_pin_a(p24);
DigitalIn g_encoder_left_pin_b(p23);
InterruptIn g_encoder_right_pin_a(p26);
DigitalIn g_encoder_right_pin_b(p25);
AnalogIn g_battery(p19);

struct SpeedPair {
  unsigned char left;
  unsigned char right;
};

/* function prototypes */
void parseRequest(const RequestMessage &req);
bool sendResponse(TCPSocket &client);
void tickLeft();
void tickRight();
void pid();
void setSpeeds(SpeedPair speed);
void bothMotorStop();
void triggerEstop();

/* desired motor speed (as specified by the client) */
float g_desired_speed_l = 0;
float g_desired_speed_r = 0;

/* actual motor speeds */
float g_actual_speed_l = 0;
float g_actual_speed_r = 0;

/* PID calculation values */
long g_last_cmd_time = 0;
int g_last_loop_time = 0;
float g_error_l = 0;
float g_error_r = 0;
float g_d_error_l = 0;
float g_d_error_r = 0;
float g_i_error_l = 0;
float g_i_error_r = 0;
float g_d_t_sec = 0;
float g_last_error_l = 0;
float g_last_error_r = 0;
float g_actual_speed_last_l = 0;
float g_actual_speed_last_r = 0;
float g_low_passed_pv_l = 0;
float g_low_passed_pv_r = 0;

/* PID constants */
float g_p_l = 0;
float g_d_l = 0;
float g_p_r = 0;
float g_d_r = 0;
float g_i_l = 0;
float g_i_r = 0;
float g_kv_l = 0;
float g_kv_r = 0;

int g_d_pwm_l = 0;
int g_d_pwm_r = 0;
int g_pwm_l = 0;
int g_pwm_r = 0;
int g_e_stop_output;

SpeedPair g_speed_pair;
uint32_t g_left_output;
uint32_t g_right_output;

/* encoder values */
volatile int g_tick_data_right = 0;
volatile int g_tick_data_left = 0;

/* calculation constants */
const double g_wheel_circum = 1.092;
const double g_gear_ratio = 32.0;
const int g_ticks_per_rev = 48;
const double g_meters_per_tick = g_wheel_circum / (g_ticks_per_rev * g_gear_ratio);

/* estop logic */
int g_estop = 1;

int main() {
  //    /* Read PCON register */
//  printf("PCON: 0x%x\n", *((unsigned int *)0x400FC180));
//  *(unsigned int *)0x400fc180 |= 0xf;

  /* Open the server (mbed) via the EthernetInterface class */
  printf("Setting up ethernet interface...\r\n");
  EthernetInterface net;
  net.connect();

  TCPSocket socket;
  socket.open(&net);
  char const *ip = "192.168.1.20"; // server ip address
  socket.connect(ip);
  printf("Server IP Address is: %s\r\n", ip);

  /* Instantiate a TCP Socket to function as the server and bind it to the
   * specified port */
  TCPSocket server_socket;
  server_socket.bind(ECHO_SERVER_PORT);
  server_socket.listen();

  g_my_led1 = 1;

  // Set interrupt function
  g_encoder_left_pin_a.rise(&tickLeft);
  g_encoder_right_pin_a.rise(&tickRight);

  wait(0.5);
  g_my_led1 = 0;

  g_timer.reset();
  g_timer.start();

  /* Instantiage a TCP socket to serve as the client */
  TCPSocket *client;

  while (true) {
    g_my_led2 = 1;
    /* wait for a new TCP Connection */
    printf("Waiting for new connection...\r\n");
    client = server_socket.accept();
    g_my_led2 = 0;

    printf("accepted new client\r\n");
    SocketAddress socket_address;
    client->getpeername(&socket_address);
    printf("Connection from: %s\r\n", socket_address.get_ip_address());
    g_estop = 1;

    while (true) {
      /* read data into the buffer. This call blocks until data is read */
      char buffer[BUFFER_SIZE];
      int n = client->recv(buffer, sizeof(buffer) - 1);

      /*
      n represents the response message for the read() command.
      - if n == 0 then the client closed the connection
      - otherwise, n is the number of bytes read
      */
      if (n < 0) {
        if (DEBUG) {
          printf("Received empty buffer\n");
        }
        wait_ms(10);
        continue;
      } if (n == 0) {
        printf("Client Closed Connection\n");
        break;
      } 
        if (DEBUG) {
          printf("Received Request of size: %d\n", n);
        }
      

      /* protobuf message to hold request from client */
      RequestMessage request = RequestMessage_init_zero;
      bool istatus;

      /* Create a stream that reads from the buffer. */
      pb_istream_t istream =
          pb_istream_from_buffer(reinterpret_cast<uint8_t *>(buffer), n);

      /* decode the message */
      istatus = pb_decode(&istream, RequestMessage_fields, &request);

      /* check for any errors.. */
      if (!istatus) {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&istream));
        continue;
      }

      parseRequest(request);

      /* reset the timer */
      if (g_timer.read_ms() > pow(2, 20)) {
        g_timer.reset();
        g_last_cmd_time = 0;
      }

      /* estop logic */
      if (g_e_stop_status.read() == 0) {
        triggerEstop();
      } else {
        g_estop = 1;
        g_e_stop_light = 0;
      }

      /* update motor velocities with PID */
      pid();

      if (!sendResponse(*client)) {
        printf("Couldn't send response to client!\r\n");
        continue;
      }
    }
    printf("Closing rip..\r\n");
    triggerEstop();
    client->close();
  }
}

bool sendResponse(TCPSocket &client) {
  /* protocol buffer to hold response message */
  ResponseMessage response = ResponseMessage_init_zero;

  /* This is the buffer where we will store the response message. */
  uint8_t responsebuffer[256];
  size_t response_length;
  bool ostatus;

  /* Create a stream that will write to our buffer. */
  pb_ostream_t ostream =
      pb_ostream_from_buffer(responsebuffer, sizeof(responsebuffer));

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

  if (DEBUG) {
    printf("Sending message of length: %zu\n", response_length);
  }

  /* Then just check for any errors.. */
  if (!ostatus) {
    printf("Encoding failed: %s\n", PB_GET_ERROR(&ostream));
    return false;
  }

  client.send(reinterpret_cast<char *>(responsebuffer), response_length);
  return true;
}

void triggerEstop() {
  // If get 5V, since inverted, meaning disabled on motors
  g_estop = 0;
  g_desired_speed_l = 0;
  g_desired_speed_r = 0;
  g_pwm_l = 0;
  g_pwm_r = 0;
  g_i_error_l = 0;
  g_i_error_r = 0;
  bothMotorStop();
  g_e_stop_light = 1;
}

/*
Update global variables using most recent client request.
@param[in] req RequestMessage protobuf with desired values
*/
void parseRequest(const RequestMessage &req) {
  /* request contains PID values */
  if (req.has_p_l) {
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
  if (req.has_speed_l) {
    g_desired_speed_l = req.speed_l;
    g_desired_speed_r = req.speed_r;
  }
}

void tickRight() {
  if (g_encoder_right_pin_a.read() == g_encoder_right_pin_b.read()) {
    ++g_tick_data_right;
  } else {
    --g_tick_data_right;
  }
}

void tickLeft() {
  if (g_encoder_left_pin_a.read() == g_encoder_left_pin_b.read()) {
    ++g_tick_data_left;
  } else {
    --g_tick_data_left;
  }
}

// https://en.wikipedia.org/wiki/PID_controller#Discrete_implementation but with
// e(t) on velocity, not position Changes to before 1: Derivative on PV 2:
// Corrected integral 3: Low pass on Derivative 4: Clamping on Integral 5: Feed
// forward
void pid() {
  // 1: Calculate dt
  g_d_t_sec = static_cast<float>(g_timer.read_ms() - g_last_loop_time) / 1000.0;

  if (g_timer.read() >= 1700) {
    g_timer.reset();
    g_last_loop_time = 0;
  }

  g_last_loop_time = g_timer.read_ms();

  // 2: Convert encoder values into velocity
  g_actual_speed_l = (g_meters_per_tick * g_tick_data_left) / g_d_t_sec;
  g_actual_speed_r = (g_meters_per_tick * g_tick_data_right) / g_d_t_sec;

  g_tick_data_left = 0;
  g_tick_data_right = 0;

  // 3: Calculate error
  g_error_l = g_desired_speed_l - g_actual_speed_l;
  g_error_r = g_desired_speed_r - g_actual_speed_r;

  // 4: Calculate Derivative Error
  // TODO(oswinso): Make alpha a parameter
  float alpha = 0.75;
  g_low_passed_pv_l = alpha * (g_actual_speed_last_l - g_actual_speed_l) / g_d_t_sec +
                    (1 - alpha) * g_low_passed_pv_l;
  g_low_passed_pv_r = alpha * (g_actual_speed_last_r - g_actual_speed_r) / g_d_t_sec +
                    (1 - alpha) * g_low_passed_pv_r;

  g_d_error_l = g_low_passed_pv_l;
  g_d_error_r = g_low_passed_pv_r;

  // 5: Calculate Integral Error
  // 5a: Calculate Error
  g_i_error_l += g_error_l * g_d_t_sec;
  g_i_error_r += g_error_r * g_d_t_sec;

  // 5b: Perform clamping
  // TODO(oswinso): make clamping a parameter
  float i_clamp = 60 / g_i_l;
  g_i_error_l = min(i_clamp, max(-i_clamp, g_i_error_l));
  g_i_error_r = min(i_clamp, max(-i_clamp, g_i_error_r));

  // 6: Sum P, I and D terms
  float feedback_left = g_p_l * g_error_l + g_d_l * g_d_error_l + g_i_l * g_i_error_l;
  float feedback_right = g_p_r * g_error_r + g_d_r * g_d_error_r + g_i_r * g_i_error_r;

  // 7: Calculate feedforward
  float feedforward_left = g_kv_l * g_desired_speed_l;
  float feedforward_right = g_kv_r * g_desired_speed_r;

  // Apparently motor commands are inverted somehow
  g_pwm_l = -static_cast<int>(round(feedforward_left + feedback_left));
  g_pwm_r = -static_cast<int>(round(feedforward_right + feedback_right));

  // Map from 1 - 127, since 0 causes both motors to stop
  g_pwm_l = min(63, max(-63, g_pwm_l)) + 64;
  g_pwm_r = min(63, max(-63, g_pwm_r)) + 64;

  // 8: Deadband
  if (abs(g_actual_speed_l) < 0.16 && abs(g_desired_speed_l) < 0.16) {
    g_pwm_l = 64;
  }

  if (abs(g_actual_speed_r) < 0.16 && abs(g_desired_speed_r) < 0.16) {
    g_pwm_r = 64;
  }

  if (g_pwm_l < 40 && g_pwm_r < 40) {
    g_my_le_d4 = 1;
    g_pwm_l = 64;
    g_pwm_r = 64;
  }

  if (g_actual_speed_l < -0.5 && g_actual_speed_r < -0.5) {
    g_my_le_d3 = 1;
    g_pwm_l = 64;
    g_pwm_r = 64;
  }

  g_speed_pair = {static_cast<unsigned char>(g_pwm_l), static_cast<unsigned char>(g_pwm_r)};
  setSpeeds(g_speed_pair);

  g_left_output = static_cast<uint32_t>(g_pwm_l);
  g_right_output = static_cast<uint32_t>(g_pwm_r);

  /*
      Be aware that this motor board does not interface with the motor
      controller with PWM. "PWM" here are mere residue from old arduino code
      -255 to 255 values are handled in motor.cpp file, mapped to 0 to 127.
  */

  g_last_error_l = g_error_l;
  g_last_error_r = g_error_r;
  g_actual_speed_last_l = g_actual_speed_l;
  g_actual_speed_last_r = g_actual_speed_r;
}

void bothMotorStop() { g_serial.putc(0); }

/**
 * Sets speed for both motors. Right motor is 0 - 127, Left motor is 128 - 255
 * @param c
 */
void setSpeeds(SpeedPair speed) {
  // Right motor
  g_serial.putc(speed.right);

  // Left motor
  g_serial.putc(static_cast<char>(128) + speed.left);
}
