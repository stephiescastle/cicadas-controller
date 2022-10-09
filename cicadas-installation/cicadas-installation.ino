// https://github.com/alextaujenis/RBD_Motor/blob/master/examples/forward_and_reverse/forward_and_reverse.ino
#include <RBD_Timer.h>
#include <RBD_Motor.h>


/**************************************

  control dc motor with pwm
  pin mapping key: https://docs.google.com/spreadsheets/d/1_K7wllq-jH08pGu8_E4yZduzdHvDAcWqfF-e0n92yMI/edit#gid=0

  - realtime timer, with flexibility to change scale of time
  - a lifecycle with variability within 
    - ramp all up and all down with a period of sustaining.
    - in the sustaining period, there is slight movements up and down per motor (randomized)
    - a lifecycle is all of the motors doing this for a period of time
    - one ball per that is extra noisy? (the last one?)
  - main board button - trigger lifecycle button
  - main board switch - toggle between using programmed cycle vs waiting for controller inputs
  - main board intensity limit knob
  - main board time scale knob
  - peripheral switch for knobs - use knobs to set intensity limit per motor. (how to save state though?)
  - peripheral switch for switches - override with motor switches (everything else is preprogrammed)

**************************************/

// Basic motor control (12)
static const uint8_t motorDriverPins[] = {2,3,4,5,6,7,8,9,10,11,12,13};
static const uint8_t motorTogglePins[] = {22,23,24,25,26,27,28,29,30,31,32,33};
static const uint8_t motorKnobPins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11};

// Switches
// switchPins[0] = main board button
// switchPins[1] = main board switch
// switchPins[2] = peripheral switch for switches (override motor on/off) 
// switchPins[3] = peripheral swich for knobs (calibration)
static const uint8_t switchPins[] = {34,35,36,37};

// Knobs
// knobPins[0] = main board intensity knob
// knobPins[1] = main board time scale knob
// knobPins[2] = peripheral j-connector input
static const uint8_t knobPins[] = {A12,A13,A14};

// Reading and storing values
int motorKnobValues[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int motorToggleValues[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int knobValues[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int switchValues[] = {-1,-1,-1,-1};

// Timing
// some stuff here: https://forum.arduino.cc/t/ramp-to-speed-over-defined-duration/562652/6
// ramp up time: https://forum.arduino.cc/t/trying-to-make-ramp-up-timer/576501/3
// RAMP library https://github.com/siteswapjuggler/RAMP/issues/19 
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
float timeScale = 1.0; // this should be knob-changeable // TODO: consider changing to int and changing ramp times to seconds (scale would be 1000)
long rampUpTime = 5000; // TODO: this should eventually be based on the timescale
long sustainTime = 10000; // TODO: this should eventually be based on the timescale
long rampDownTime = 5000; // TODO: this should eventually be based on the timescale
int restInterval = 10000; // TODO: use timescale
int initialRate = 0;
int finalRate = 150; // TODO: this should be knob-changeable

void setup() {
  // setup digital pins
  for (int i = 0; i < 12; i++) {
    pinMode(motorDriverPins[i], OUTPUT);
  }
  for (int i = 0; i < 12; i++) {
    pinMode(motorTogglePins[i], INPUT);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(switchPins[i], INPUT);
  }
  // this particular one is buggy
  pinMode(motorTogglePins[6], INPUT_PULLUP);

  // start serial communications at 9600 baud
  // Serial.begin(9600);

}

void loop() {
  // set time
  currentMillis = millis();
  if (currentMillis - previousMillis >= restInterval) {
    previousMillis = currentMillis;
    do {
      brightness1 = brightness1 + fadeAmount;
    }
    while (brightness1 > pwm1);
  }

  // read knobPins
  for (int i = 0; i < 3; i++) {
    knobValues[i] = map(analogRead(knobPins[i]), 0, 1023, 0, 255);
    // Serial.print("knobValues[");
    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.println(knobValues[i]);
  }
  // read switchPins
  for (int i = 0; i < 4; i++) {
    switchValues[i] = digitalRead(switchPins[i]);
    // Serial.print("switchValues[");
    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.println(switchValues[i]);
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
      if (switchValues[1] == 1) {
        // if main board override, use main board knob for all
        analogWrite(motorDriverPins[i], knobValues[0]);
      } else {
        analogWrite(motorDriverPins[i], motorKnobValues[i]);
      }
    } else {
      // off
      analogWrite(motorDriverPins[i], 0);
    }
  }

}
