#include <SPI.h>
#include <RH_RF95.h>
#include <string.h>

#define RFM95_CS 10 // CE, CSN
#define RFM95_RST 2 // top left pin, top row
#define RFM95_INT 3 // interrupt pin
#define RF95_FREQ 915.0 // Change to 434.0 or other frequency, must match RX's freq!

RH_RF95 rf95(RFM95_CS, RFM95_INT); // Singleton instance of the radio driver

int status_led = 5;
int out_pin = 9;

void setup() {

  bool setupFailure = true;
  
  while (setupFailure) {
    pinMode(RFM95_RST, OUTPUT);
    pinMode(status_led, OUTPUT);
    pinMode(out_pin, OUTPUT);
    
    digitalWrite(RFM95_RST, HIGH);
    digitalWrite(status_led, LOW);
    
    // manual reset
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);
  
    while (!rf95.init()) {
      break;
    }
  
    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
    if (!rf95.setFrequency(RF95_FREQ)) {
      break;
    }
  
    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  
    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
    // you can set transmitter powers from 5 to 23 dBm:
    rf95.setTxPower(23, false);

    setupFailure = false;
  }

  fastBlink(10);
}

void loop() {
  
  // digitalWrite(status_led, HIGH);
  if (rf95.available()){
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len)){
      // digitalWrite(status_led, HIGH);

      // RH_RF95::printBuffer("Received: ", buf, len);

      if (strcmp((char *)buf, "GOGO") == 0) {
        // digitalWrite(status_led, HIGH);
        // fastBlink(30);
        digitalWrite(out_pin, LOW);
      } else if (strcmp((char *)buf, "STOP") == 0) {
        digitalWrite(status_led, LOW);
        digitalWrite(out_pin, HIGH);
      } else {
        // blink(5);
      }

      /*
      // Send a reply
      uint8_t data[] = "received";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      */
      
      // fastBlink(30); 
      } else {
        // blink(3);
      }
    } else {
      // blink(10); 
    }
}

void blink(int numTimes) {

  volatile int j = 0;
  
  for (int i = 0; i < numTimes; i++) {
    
    digitalWrite(status_led, HIGH);
    for (j = 0; j < 10000; j++) {}
    digitalWrite(status_led, LOW);
    for (j = 0; j < 10000; j++) {}
  }
}

void fastBlink(int numTimes) {

  volatile int j = 0;
  
  for (int i = 0; i < numTimes; i++) {
    
    digitalWrite(status_led, HIGH);
    for (j = 0; j < 1000; j++) {}
    digitalWrite(status_led, LOW);
    for (j = 0; j < 1000; j++) {}
  }
}
