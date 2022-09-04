/**************************************

  control dc motor with pwm
  pin mapping key: https://docs.google.com/spreadsheets/d/1_K7wllq-jH08pGu8_E4yZduzdHvDAcWqfF-e0n92yMI/edit#gid=0

**************************************/
static const uint8_t motorDriverPins[] = {2,3,4,5,6,7,8,9,10,11,12,13};
static const uint8_t motorTogglePins[] = {22,23,24,25,26,27,28,29,30,31,32,33};
static const uint8_t parameterTogglePins[] = {34,35,36,37};
static const uint8_t motorKnobPins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11};
static const uint8_t parameterKnobPins[] = {A12,A13,A14};

int motorKnobValues[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int parameterKnobValues[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int motorToggleValues[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int parameterToggleValues[] = {-1,-1,-1,-1};

void setup() {
  // setup digital pins
  for (int i = 0; i < 12; i++) {
    pinMode(motorDriverPins[i], OUTPUT);
  }
  for (int i = 0; i < 12; i++) {
    pinMode(motorTogglePins[i], INPUT);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(parameterTogglePins[i], INPUT);
  }
  // this particular one is buggy
  pinMode(motorTogglePins[6], INPUT_PULLUP);

  // start serial communications at 9600 baud
  // Serial.begin(9600);

}

void loop() {
  // read parameterKnobPins
  for (int i = 0; i < 3; i++) {
    parameterKnobValues[i] = map(analogRead(parameterKnobPins[i]), 0, 1023, 0, 255);
    // Serial.print("parameterKnobValues[");
    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.println(parameterKnobValues[i]);
  }
  // read parameterTogglePins
  for (int i = 0; i < 4; i++) {
    parameterToggleValues[i] = digitalRead(parameterTogglePins[i]);
    // Serial.print("parameterToggleValues[");
    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.println(parameterToggleValues[i]);
  }
  // read motorKnobPins 
  for (int i = 0; i < 12; i++) {
    motorKnobValues[i] =  map(analogRead(motorKnobPins[i]), 0, 1023, 0, 255);
    // Serial.print("motorKnobValues[");
    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.println(motorKnobValues[i]);
  }
  // read motorTogglePins 
  for (int i = 0; i < 12; i++) {
    motorToggleValues[i] = digitalRead(motorTogglePins[i]);
    // Serial.print("motorToggleValues[");
    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.println(motorToggleValues[i]);
  }

  // evaluate and do
  for (int i = 0; i < 12; i++) {
    if (motorToggleValues[i] == 1) {
      // drive motor if switched on
      if (parameterToggleValues[1] == 1) {
        // if main board override, use main board knob for all
        analogWrite(motorDriverPins[i], parameterKnobValues[0]);
      } else {
        analogWrite(motorDriverPins[i], motorKnobValues[i]);
      }
    } else {
      // off
      analogWrite(motorDriverPins[i], 0);
    }
  }

}
