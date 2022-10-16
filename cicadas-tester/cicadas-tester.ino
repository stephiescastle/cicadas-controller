/**************************************

  Simple patch to test cicada boards
  
  pin mapping key: https://docs.google.com/spreadsheets/d/1_K7wllq-jH08pGu8_E4yZduzdHvDAcWqfF-e0n92yMI/edit#gid=0

**************************************/
static const uint8_t motorDriverPin[] = {2,3,4,5,6,7,8,9,10,11,12,13};
static const uint8_t motorTogglePin[] = {22,23,24,25,26,27,28,29,30,31,32,33};
static const uint8_t motorKnobPin[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11};
static const uint8_t knobPin[] = {A12,A13,A14};
static const uint8_t switchPin[] = {34,35,36,37};

int motorKnobValue[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int motorToggleValue[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int knobValue[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int switchValue[] = {-1,-1,-1,-1};

void setup() {
  // setup digital pins
  for (int i = 0; i < 12; i++) {
    pinMode(motorDriverPin[i], OUTPUT);
  }
  for (int i = 0; i < 12; i++) {
    pinMode(motorTogglePin[i], INPUT);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(switchPin[i], INPUT);
  }
  // this particular one is buggy
  // pinMode(motorTogglePin[6], INPUT_PULLUP);

  // start serial communications at 9600 baud
  Serial.begin(9600);

}

void loop() {
  // read knobPin
  for (int i = 0; i < 3; i++) {
    knobValue[i] = map(analogRead(knobPin[i]), 0, 1023, 0, 255);
    Serial.print("knobValue[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(knobValue[i]);
  }
  // read switchPin
  for (int i = 0; i < 4; i++) {
    switchValue[i] = digitalRead(switchPin[i]);
    Serial.print("switchValue[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(switchValue[i]);
  }
  // read motorKnobPin 
  for (int i = 0; i < 12; i++) {
    motorKnobValue[i] =  map(analogRead(motorKnobPin[i]), 0, 1023, 0, 255);
//    Serial.print("motorKnobValue[");
//    Serial.print(i);
//    Serial.print("] = ");
//    Serial.println(motorKnobValue[i]);
  }
  // read motorTogglePin 
  for (int i = 0; i < 12; i++) {
    motorToggleValue[i] = digitalRead(motorTogglePin[i]);
//    Serial.print("motorToggleValue[");
//    Serial.print(i);
//    Serial.print("] = ");
//    Serial.println(motorToggleValue[i]);
  }
  
  // do something
  for (int i = 0; i < 12; i++) {
    if (motorToggleValue[i] == 1) {
      // check if motor is switched on first (off if no controller board)
      if (switchValue[1] == 1) {
        // if main board override, use main board knob for all
        analogWrite(motorDriverPin[i], knobValue[0]);
      } else {
        analogWrite(motorDriverPin[i], motorKnobValue[i]);
      }
    } else {
      // off
      analogWrite(motorDriverPin[i], 0);
    }
  }
}
