#include <RadioHead.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <string.h>
#include <math.h>

#define RFM95_CS 10 // level switch for SPI
#define RFM95_RST 2 // reset pin for the radio
#define RFM95_INT 3 // interrupt
#define RF95_FREQ 915.0 // Change to 434.0 or other frequency, must match RX's freq!
#define NUM_TIMES_RESEND 20

#define MAX_DOWN_COUNT 20

#define DEADBAND 2.0

int wireless_led = 5; // Wireless led, lights up if there is communication
int button_led = 6; // Button led, shows status of the button
int button_pin = 8; // Button pin

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int buttonState;
int buttonFlag;
int estopOffState;

void setup() {
  
  bool setupFailure = true;
    while (setupFailure) {
      pinMode(button_led, OUTPUT);
      pinMode(wireless_led, OUTPUT);
      pinMode(RFM95_RST, OUTPUT);
      pinMode(button_pin, INPUT);

      digitalWrite(button_led, LOW);
      digitalWrite(wireless_led, LOW);
    
      // manual reset
      digitalWrite(RFM95_RST, LOW);
      delay(10);
      digitalWrite(RFM95_RST, HIGH);
      delay(10);
    
      // initialize the radio
      if (!rf95.init()) {
        break;
      }
    
      // set the frequency
      if (!rf95.setFrequency(RF95_FREQ)) {
        break;
      }

      rf95.setTxPower(23);

      setupFailure = false;
    }

    estopOffState = digitalRead(button_pin);
}

void loop() {
  
  char radiopacket[5];

  buttonState = digitalRead(button_pin);

/*
  if (buttonState != estopOffState) {
    if (downCount < MAX_DOWN_COUNT) {
      downCount++;
    } else {
      buttonFlag = 1;
    }
  } else {
    downCount = 0;
  }
  */

  
  if (buttonState != estopOffState) {
    buttonFlag = 1;
  } else {
    buttonFlag = 0;
  }
  

  if(buttonFlag){
    digitalWrite(button_led, HIGH);
    memcpy(radiopacket, "STOP", 5);
    
    buttonFlag = 0;
  } else {
    digitalWrite(button_led, LOW);
    memcpy(radiopacket, "GOGO", 5);

    estopOffState = buttonState;
  }

  rf95.send((uint8_t *)radiopacket, 5);
  rf95.waitPacketSent();

  /*
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  
  if (rf95.waitAvailableTimeout(1000)){ 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len) && len != 0){
      digitalWrite(wireless_led, HIGH);
    } else {
      digitalWrite(wireless_led, LOW);
    }
  }
  */

  delay(5);
}
