/**************************************

  control dc motor with pwm
  pin mapping key: https://docs.google.com/spreadsheets/d/1_K7wllq-jH08pGu8_E4yZduzdHvDAcWqfF-e0n92yMI/edit#gid=0

  - realtime timer, with flexibility to change scale of time
  - a lifecycle with variability within
    - ramp all up and all down with a period of sustaining.
    - TODO: in the sustaining period, there is slight movements up and down per motor (randomized)
    - a lifecycle is all of the motors doing this for a period of time
    - one ball per that is extra noisy? (the last one?)
  - TODO: main board button - trigger lifecycle button
  - TODO: main board switch - toggle between using programmed cycle vs waiting for controller inputs, or toggle between knob modes
  - main board intensity limit knob
  - main board time scale knob
  - TODO: peripheral switch for knobs - use knobs to set intensity limit per motor. (how to save state though?)
  - TODO: peripheral switch for switches - override with motor switches (everything else is preprogrammed)

**************************************/

#include <RBD_Timer.h>
#include <RBD_Motor.h>

/* VARS & CLASSES -------------------------------------------- */

// ---- Limits, Constants, States ---- //
// if brood is awake or sleeping
bool awake = false;
// sleep duration (s)
unsigned long sleepTime = 6;
// awake duration  (s)
unsigned long awakeTime = 24;
// time scale (ms)
long timeScale = 1000;
// min timeScale (ms);
int timeScaleMin = 500;
// max timeScale (ms);
int timeScaleMax = 2000;
// base ramp time for motors (ms) TODO: change to (s)?
unsigned long rampBasis = 6000;
// min motor ramp time (s)
float motorRampMin = 0.7;
// max motor ramp time (s)
float motorRampMax = 1.7;
// randomization range (+/- s)
float rampFactor = 0.076;
// motor intensity
int intensity = 150;
int intensityMax = 255;

// ---- Individual Motor/Cicada States ---- //
// number of motors in arrays
static const uint8_t motorCount = 12;
// motor pins
// static const uint8_t motorDriverPin[] = {2,3,4,5,6,7,8,9,10,11,12,13};
RBD::Motor motor[] = {(2),(3),(4),(5),(6),(7),(8),(9),(10),(11),(12),(13)};
// ramp direction for each per motor
bool rampingUp[] = {true, true, true, true, true, true, true, true, true, true, true, true}; 
// ramp durations for each motor (ms)
unsigned long rampTime[] = {rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis};
// create and set timer for each motor
RBD::Timer rampTimer[] = {rampTime[0], rampTime[1], rampTime[2], rampTime[3], rampTime[4], rampTime[5], rampTime[6], rampTime[7], rampTime[8], rampTime[9], rampTime[10], rampTime[11]};

// ---- Motor Control Inputs ---- //
static const uint8_t motorTogglePin[] = {22,23,24,25,26,27,28,29,30,31,32,33};
static const uint8_t motorKnobPin[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11};

// ---- General Switches ---- //
// switchPin[0] = main board button
// switchPin[1] = main board switch
// switchPin[2] = peripheral switch for switches (override motor on/off) 
// switchPin[3] = peripheral swich for knobs (calibration)
static const uint8_t switchPin[] = {34,35,36,37};

// ---- General Knobs ---- //
// knobPin[0] = main board intensity knob
// knobPin[1] = main board time scale knob
// knobPin[2] = peripheral j-connector input
static const uint8_t knobPin[] = {A12,A13,A14};

// ---- Reading / Storing Values ---- //
int motorKnobValue[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int motorToggleValue[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int knobValue[] = {-1,-1,-1};
int switchValue[] = {-1,-1,-1,-1};

// ---- Global Timers ---- //
// sleep timer
RBD::Timer sleepTimer(sleepTime*timeScale);
// awake timer
RBD::Timer awakeTimer(awakeTime*timeScale);

/* UTIL FUNCTIONS ------------------------------------------- */

// return the desired pwm depending on other factors
int motorSpeed(bool varied = false) {
  if(awake) {
    intensity = map(knobValue[0], 0, 1023, 0, intensityMax);
    if (varied) {
      return variedIntensity();
    } else {
      return intensity;
    }
  } else {
    return 0;
  }
}

// vary the target intensity (pwm) by reducing with a varying percentage of itself (between 25-75%)
long variedIntensity() {
  return intensity - int(float(intensity) * (float(random(25,76))*.01));
}

// alter ramp time
void randomizeRampTime(int i) {
  int factor = random(int(rampFactor * float(timeScale)));
  if (random(2) == 0) {
    factor = factor * (-1);
  }
  rampTime[i] = constrain(rampTime[i] + factor, int(motorRampMin * float(timeScale)), int(motorRampMax * float(timeScale)));
}

void ramp(int i, bool up = false) {
  // up = true --> ramp up
  // up = false --> ramp down
  rampingUp[i] = up;
  randomizeRampTime(i);
  rampTimer[i].setTimeout(rampTime[i]);
  rampTimer[i].restart();
  if (awake) {
    if (up) {
      motor[i].ramp(motorSpeed(), rampTime[i]);
    } else {
      // explore making the ramp timer shorter for the sustain period
      // use varied motorSpeed
      motor[i].ramp(motorSpeed(true), rampTime[i]);
      Serial.print("Sustain PWM for [");
      Serial.print(i);
      Serial.print("]: ");
      Serial.println(variedIntensity());
    }
  } else {
    // off
    motor[i].ramp(0, rampTime[i]);
  }
}

/* SETUP ---------------------------------------------------- */

void setup() {
  Serial.begin(9600);
  // reset randomizers w/ unused analog pin
  randomSeed(analogRead(A15));

  // setup digital pins
  for (int i = 0; i < 12; i++) {
    pinMode(motorTogglePin[i], INPUT);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(switchPin[i], INPUT);
  }

  // set timeScale
  timeScale = map(knobValue[1], 0, 1023, timeScaleMin, timeScaleMax);
  Serial.print("TIMESCALE SETUP: ");
  Serial.println(timeScale);

  // start asleep
  sleepTimer.setTimeout(sleepTime * timeScale);
  sleepTimer.restart();
}

/* LOOP ----------------------------------------------------- */

void loop() {

  // read knobPin
  for (int i = 0; i < 3; i++) {
    knobValue[i] = analogRead(knobPin[i]);
    // Serial.print("knobValue[");
    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.println(knobValue[i]);
  }
  // read switchPin
  for (int i = 0; i < 4; i++) {
    switchValue[i] = digitalRead(switchPin[i]);
    // Serial.print("switchValue[");
    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.println(switchValue[i]);
  }
  // read motorKnobPin 
  for (int i = 0; i < motorCount; i++) {
    motorKnobValue[i] =  map(analogRead(motorKnobPin[i]), 0, 1023, 0, 255);
  }
  // read motorTogglePin 
  for (int i = 0; i < motorCount; i++) {
    motorToggleValue[i] = digitalRead(motorTogglePin[i]);
  }
  
  // -- Evaluate & Do -- //

  // update timescale
  timeScale = map(knobValue[1], 0, 1023, timeScaleMin, timeScaleMax);

  if(sleepTimer.onExpired()) {
    Serial.print("TIMESCALE: ");
    Serial.println(timeScale);

    Serial.print("Waking for ");
    Serial.println(awakeTime * timeScale);

    awake = true;
    awakeTimer.setTimeout(awakeTime * timeScale);
    awakeTimer.restart();
  }

  for (int i = 0; i < motorCount; i++) {
    if(rampTimer[i].onExpired()){
      if (rampingUp[i]) {
        // ramp down
        ramp(i);
      } else {
        // ramp up
        ramp(i, true);
      }
    }
  }

  if (awakeTimer.onExpired()) {
    // ramp down anything in progress
    for (int i = 0; i < motorCount; i++) {
      ramp(i);
    }
    // go to sleep
    awake = false;
    sleepTimer.setTimeout(sleepTime * timeScale);
    sleepTimer.restart();
    Serial.print("Sleeping for ");
    Serial.println(sleepTime * timeScale);
  }

  // -- Controller board -- //
  // TODO: Check if there's a controller board
  // TODO: This will override any of the timing stuff above, so I need to section it off
  // for (int i = 0; i < 12; i++) {
  //   if (motorToggleValue[i] == 1) {
  //     // check if motor is switched on first (off if no controller board)
  //     if (switchValue[1] == 1) {
  //       // if main board override, use main board knob for all
  //       motor[i].setSpeed(map(knobValue[0], 0, 1023, 0, 255));
  //       // analogWrite(motorDriverPin[i], knobValue[0]);
  //     } else {
  //       motor[i].setSpeed(motorKnobValue[i]);
  //       // analogWrite(motorDriverPin[i], motorKnobValue[i]);
  //     }
  //   } else {
  //     // off
  //     motor[i].setSpeed(0);
  //     // analogWrite(motorDriverPin[i], 0);
  //   }
  // }
  // -- End controller board -- //

  // REQUIRED FOR ALL -- update motors
  for (int i = 0; i < motorCount; i++) {
    motor[i].update();
  }

}
