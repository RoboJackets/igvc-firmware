//int INPUT2 = 7;
//int INPUT1 = 8;
//int INPUT3 = 12;
//int INPUT4 = 13;  
#define M_RIGHT     3
#define M_LEFT      2
#define SPEED_IN    9
#define TURN_IN     10

int RightMotor = 0;
int LeftMotor = 0;
int speedInput = 0;
int rightMotorOutput = 0;
int leftMotorOutput = 0;

void setup() {
  Serial.begin(9600);

  pinMode(SPEED_IN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(M_RIGHT, OUTPUT);
  pinMode(M_LEFT, OUTPUT);

  digitalWrite(M_LEFT, LOW);
  digitalWrite(M_RIGHT, LOW);
}

void loop() {
//  channel1Input = map(pulseIn(CH1_IN, HIGH), 1100, 1900, 0, 255);
//
//  if (channel1Input > 100) {
//    digitalWrite(LED_BUILTIN, HIGH);
//  } else {
//    digitalWrite(LED_BUILTIN, LOW);
//  }
//  
//  delay(20);

  
  speedInput = map(pulseIn(SPEED_IN, HIGH), 1100, 1900, 0, 255);
  rightMotorOutput = speedInput - map(pulseIn(TURN_IN, HIGH), 1000, 2000, -70, 70);
  leftMotorOutput = speedInput - map(pulseIn(TURN_IN, HIGH), 1000, 2000, +70, -70);

  if (rightMotorOutput < 0) {
    rightMotorOutput = 0;
  }
  if (leftMotorOutput < 0) {
    leftMotorOutput = 0;
  }

  analogWrite(M_RIGHT, rightMotorOutput);
  analogWrite(M_LEFT, rightMotorOutput);
  
//  Serial.println(channel1Input);
//  RightMotor = SpeedValue - map(pulseIn(10, HIGH), 1000, 2000, -70, 70);
//  LeftMotor = SpeedValue - map(pulseIn(10, HIGH), 1000, 2000, +70, -70);
//  Serial.print("RIGHT MOTOR: ");
//  Serial.print(RightMotor);
//  Serial.print("  LEFT MOTOR: ");
//  Serial.println(LeftMotor);
//  
//  if (RightMotor < 0) {
//    RightMotor = 0;
//  }
//  if (LeftMotor < 0) {
//    LeftMotor = 0;
//  }
//  
//  analogWrite(5, LeftMotor);
//  analogWrite(6, RightMotor);
}
