/**************************************

  control dc motor with pwm
  pin mapping key: https://docs.google.com/spreadsheets/d/1_K7wllq-jH08pGu8_E4yZduzdHvDAcWqfF-e0n92yMI/edit#gid=0

**************************************/
static const uint8_t motorKnobPins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11};
static const uint8_t parameterKnobPins[] = {A12,A13,A14};
static const uint8_t motorDriverPins[] = {2,3,4,5,6,7,8,9,10,11,12,13};
static const uint8_t motorTogglePins[] = {22,23,24,25,26,27,28,29,30,31,32,33};
static const uint8_t parameterTogglePins[] = {34,35,36,37};

int motorKnobValues[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int parameterKnobValues[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int motorToggleValues[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int parameterToggleValues[] = {-1,-1,-1,-1};

int motorPin1 = 2;
int motorPin2 = 3;
int motorPin3 = 4;
int motorPin4 = 5;
int motorPin5 = 6;
int motorPin6 = 7;
int motorPin7 = 8;
int motorPin8 = 9;
int motorPin9 = 10;
int motorPin10 = 11;
int motorPin11 = 12;
int motorPin12 = 13;

int knobPin1 = A0;
int knobPin2 = A1;
int knobPin3 = A2;

int knobValue1 = 0;
int knobValue2 = 0;
int knobValue3 = 0;

int switchPin1 = 22;
int switchValue1 = 0;

void setup() {
  // put your setup code here, which runs once

  // set digital pin as output
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(motorPin5, OUTPUT);
  pinMode(motorPin6, OUTPUT);
  pinMode(motorPin7, OUTPUT);
  pinMode(motorPin8, OUTPUT);
  pinMode(motorPin9, OUTPUT);
  pinMode(motorPin10, OUTPUT);
  pinMode(motorPin11, OUTPUT);
  pinMode(motorPin12, OUTPUT);

  pinMode(switchPin1, INPUT);

  // start serial communications at 9600 baud
  Serial.begin(9600);

}

void loop() {
  switchValue1 = digitalRead(switchPin1);
  Serial.print("Switch1: ");
  Serial.println(switchValue1);

  if (switchValue1 == 1) {
    knobValue1 = map(analogRead(knobPin1), 0, 1023, 0, 255);
    Serial.println(knobValue1);
    analogWrite(motorPin1, knobValue1);
    analogWrite(motorPin2, knobValue1);
    analogWrite(motorPin3, knobValue1);
    analogWrite(motorPin4, knobValue1);
    analogWrite(motorPin5, knobValue1);
    analogWrite(motorPin6, knobValue1);
    analogWrite(motorPin7, knobValue1);
    analogWrite(motorPin8, knobValue1);
    analogWrite(motorPin9, knobValue1);
    analogWrite(motorPin10, knobValue1);
    analogWrite(motorPin11, knobValue1);
    analogWrite(motorPin12, knobValue1);
  } else {
    analogWrite(motorPin1, 0);
    analogWrite(motorPin2, 0);
    analogWrite(motorPin3, 0);
    analogWrite(motorPin4, 0);
    analogWrite(motorPin5, 0);
    analogWrite(motorPin6, 0);
    analogWrite(motorPin7, 0);
    analogWrite(motorPin8, 0);
    analogWrite(motorPin9, 0);
    analogWrite(motorPin10, 0);
    analogWrite(motorPin11, 0);
    analogWrite(motorPin12, 0);
  }

  //  knobValue2 = map(analogRead(knobPin2), 0, 1023, 0, 255);
  //  // Serial.println(knobValue2);
  //  analogWrite(motorPin2, knobValue2);
  //
  //  knobValue3 = map(analogRead(knobPin3), 0, 1023, 0, 255);
  //  // Serial.println(knobValue3);
  //  analogWrite(motorPin3, knobValue3);

}
