#include <Stepper.h>
#include <mcp_can.h>
#include <SPI.h>

// change this to the number of steps on your motor
#define STEPS       800
#define MAX_SPEED   1300
#define CAN0_INT     2

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, 4, 5, 6, 7);
MCP_CAN CAN0(10);

// the previous reading from the analog input
int previous = 0;
long int steps = 320000;

long int currPos = 0;
long int requestedPos = 0;

// can variables
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];         


void setup() {

  Serial.begin(115200);
  
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN0_INT, INPUT);
  Serial.println("MCP2515 Library Receive Example...");

  // Stepper Stuff
  // set the speed of the motor to 30 RPMs
  stepper.setSpeed(MAX_SPEED);
  
//  for (int i = 0; i < 10; i++) {
//      stepper.step(steps * -1);
//  }
  
}


void loop() {
//  // get the sensor value
//  int val = analogRead(0);
//
//  // move a number of steps equal to the change in the
//  // sensor reading
//  stepper.step(val - previous);
//
//  // remember the previous value of the sensor
//  previous = val;

    processSteps();
//    delay(100);

//  // can action
//  readCAN();
  getInput();
}

void processSteps() {
  // bound the values
  if (requestedPos - currPos > 1000) {
    stepper.step(500);
    currPos += 500;
  } else if (requestedPos - currPos < -1000) {
    stepper.step(-500);
    currPos -= 500;
  } else {
    stepper.step(requestedPos - currPos);
    currPos = requestedPos;
  }
}

void getInput() {
//  if(!digitalRead(CAN0_INT)) {
//    CAN0.readMsgBuf(&rxId, &len, rxBuf);
//
//    float radVal = (*(float *)rxBuf);
//    long int mySteps = (long int)((radVal * 12800000) / (PI));
//
//    sprintf(msgString, "Steps: %ld\n", mySteps);
//    Serial.print(msgString);
//  }

  if(!digitalRead(CAN0_INT)) {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);

    float radVal = (*(float *)rxBuf);
//    requestedPos = (long int)((radVal * 12800000) / (PI));
    requestedPos = (long int)((radVal * 320000) / (PI));

  }
  
}

void readCAN() {
  if(!digitalRead(CAN0_INT)) {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);
    
    if((rxId & 0x80000000) == 0x80000000)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  
    Serial.print(msgString);
  
    if((rxId & 0x40000000) == 0x40000000){
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for(int i = len-1; i>=0; i--){
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);
      }
    }
        
    Serial.println();
  }
}
