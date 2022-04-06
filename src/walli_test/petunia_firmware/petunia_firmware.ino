//#define CH1_IN 9
//#define CH3_IN 10

// 1100 - 1900
#define CH1_IN  8    
#define CH2_IN  9    
#define CH3_IN  10   
#define CH4_IN  11   
#define CH5_IN  12
#define CH6_IN  13

#define EN_RIGHT 3
#define EN_LEFT 2

#define RIGHT1  5
#define RIGHT2  4
#define LEFT1   7
#define LEFT2   6

int ch1Input = 0;
int ch2Input = 0;
int ch3Input = 0;
int ch4Input = 0;
int ch5Input = 0;
int ch6Input = 0;

int rightMotor = 0;
int leftMotor = 0;

void setup() {
  Serial.begin(9600);
  pinMode(CH1_IN, INPUT);
  pinMode(CH2_IN, INPUT);
  pinMode(CH3_IN, INPUT);
  pinMode(CH4_IN, INPUT);
  pinMode(CH5_IN, INPUT);
  pinMode(CH6_IN, INPUT);

  pinMode(RIGHT1, OUTPUT);
  pinMode(RIGHT2, OUTPUT);
  pinMode(LEFT1, OUTPUT);
  pinMode(LEFT2, OUTPUT);
  pinMode(EN_RIGHT, OUTPUT);
  pinMode(EN_LEFT, OUTPUT);
}

void loop() {

//  ch1Input = map(pulseIn(CH1_IN, HIGH), 1170, 1830, 0, 255);
//  ch2Input = map(pulseIn(CH2_IN, HIGH), 1170, 1830, 0, 255);
//  ch3Input = map(pulseIn(CH1_IN, HIGH), 1170, 1830, 0, 255);
//  ch4Input = map(pulseIn(CH3_IN, HIGH), 1170, 1830, 0, 255);
//  ch5Input = map(pulseIn(CH1_IN, HIGH), 1170, 1830, 0, 255);
//  ch6Input = map(pulseIn(CH3_IN, HIGH), 1170, 1830, 0, 255);

  tankDrive();
//  testRadio();
}

void tankDrive() {
  rightMotor = map(pulseIn(CH2_IN, HIGH), 1100, 1900, -255, 255);
  leftMotor = map(pulseIn(CH3_IN, HIGH), 1100, 1900, -255, 255);

//  Serial.print("RightMotor: ");
//  Serial.print(rightMotor);
//  Serial.print(" LeftMotor: ");
//  Serial.println(leftMotor);
//  delay(200);
  
  if (rightMotor > 0) {
    digitalWrite(RIGHT1, HIGH);
    digitalWrite(RIGHT2, LOW);
    analogWrite(EN_RIGHT, rightMotor);
  } else {
    digitalWrite(RIGHT1, LOW);
    digitalWrite(RIGHT2, HIGH);
    analogWrite(EN_RIGHT, -1 * rightMotor);
  }

  if (leftMotor > 0) {
    digitalWrite(LEFT1, HIGH);
    digitalWrite(LEFT2, LOW);
    analogWrite(EN_LEFT, leftMotor);
  } else {
    digitalWrite(LEFT1, LOW);
    digitalWrite(LEFT2, HIGH);
    analogWrite(EN_LEFT, -1 * leftMotor);
  }

  
  
}

void carDrive() {
  
}

void testRadio() {
  ch1Input = pulseIn(CH1_IN, HIGH);
  ch2Input = pulseIn(CH2_IN, HIGH);
  ch3Input = pulseIn(CH3_IN, HIGH);
  ch4Input = pulseIn(CH4_IN, HIGH);
  ch5Input = pulseIn(CH5_IN, HIGH);
  ch6Input = pulseIn(CH6_IN, HIGH);
  
  Serial.print("CH1: ");
  Serial.print(ch1Input);
  Serial.print(" CH2: ");
  Serial.print(ch2Input);
  Serial.print(" CH3: ");
  Serial.print(ch3Input);
  Serial.print(" CH4: ");
  Serial.print(ch4Input);
  Serial.print(" CH5: ");
  Serial.print(ch5Input);
  Serial.print(" CH6: ");
  Serial.println(ch6Input);
  
  delay(500);
}
