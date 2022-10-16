/**************************************

  control dc motor with pwm
  pin mapping key: https://docs.google.com/spreadsheets/d/1_K7wllq-jH08pGu8_E4yZduzdHvDAcWqfF-e0n92yMI/edit#gid=0

  - realtime timer, with flexibility to change scale of time
  - a lifecycle with variability within
    - ramp all up and all down with a period of sustained cycling.
    - in the sustain period, there is slight movement up and down per motor (randomized)
    - TODO: randomize base PWM drastically (each cycle, not each ramp) - strong brood vs weaker brood
    - a lifecycle is all of the motors doing this for a period of time
    - one ball per that is extra noisy? (the last one?)
  - TODO: main board button - trigger lifecycle button
  - TODO: main board switch - toggle between using programmed cycle vs waiting for controller inputs, or toggle between knob modes
  - main board intensity limit knob
  - main board time scale knob
  - TODO: peripheral switch for knobs - use knobs to set intensity limit per motor. (how to save state though?)
  - TODO: peripheral switch for switches - override with motor switches (everything else is preprogrammed)
  - From controller: (serial print values to get sense)
    - main board switch to enable controller (nice to have)
    - realtime change sleeptime - ctrlKnobValue[0]
    - realtime change awaketime - ctrlKnobValue[1]
    - realtime change ramp time - ctrlKnobValue[2]
    - TODO: do we need intensity min? two knobs to control threshold?
  - TODOs immediate prep
    - need ability to easily change if brood starts asleep or awake (in the code)
    - random event, modulo 6 or 7 (rand) that ramps motors up and down in unison to full pwm
    - TEST - serial print scale variables to easily set these during the install
    - TEST - need ability to toggle knob inputs for changing timing settings (use hard-coded values vs use knobs) - use onboard switch (on = use knob vals)

**************************************/

#include <RBD_Timer.h>
#include <RBD_Motor.h>

/* VARS & CLASSES -------------------------------------------- */

// ---- Limits, Constants, States ---- //
// if brood is awake or sleeping
bool awake = false;
// sleep duration (s)
unsigned long sleepTime = 6; // 6
// awake duration  (s) 
unsigned long awakeTime = 24; // 24
// time scale (ms)
long timeScale = 1000; // 1000
// min timeScale (ms)- double time;
int timeScaleMin = 500;
// max timeScale (ms) - half time;
int timeScaleMax = 2000;
// base ramp time for motors (ms) TODO: change to (s)?
unsigned long rampBasis = 6000; // 6000
// min motor ramp time (s)
float motorRampMin = 0.5;
// max motor ramp time (s)
float motorRampMax = 20;
// randomization range (+/- s)
// TODO: adjust this time time scale so it's never more than actual ramp length?
float rampFactor = 1.0;
// motor intensity
int intensity = 150;
int intensityMax = 255;

// ---- Individual Motor/Cicada States ---- //
// number of motors in arrays
static const uint8_t motorCount = 12;
// motor pins
RBD::Motor motor[] = {(2),(3),(4),(5),(6),(7),(8),(9),(10),(11),(12),(13)};
// ramp direction for each per motor
bool rampingUp[] = {true, true, true, true, true, true, true, true, true, true, true, true}; 
// ramp durations for each motor (ms)
unsigned long rampTime[] = {rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis, rampBasis};
// create and set timer for each motor
RBD::Timer rampTimer[] = {rampTime[0], rampTime[1], rampTime[2], rampTime[3], rampTime[4], rampTime[5], rampTime[6], rampTime[7], rampTime[8], rampTime[9], rampTime[10], rampTime[11]};

// ---- Motor Control Inputs ---- //
static const uint8_t ctrlSwitchPin[] = {22,23,24,25,26,27,28,29,30,31,32,33};
static const uint8_t ctrlKnobPin[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11};

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
int ctrlKnobValue[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int ctrlSwitchValue[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int knobValue[] = {-1,-1,-1};
int switchValue[] = {-1,-1,-1,-1};

// ---- Global Timers ---- //
// sleep timer
RBD::Timer sleepTimer(sleepTime*timeScale);
// awake timer
RBD::Timer awakeTimer(awakeTime*timeScale);

/* UTIL FUNCTIONS ------------------------------------------- */

// return the desired pwm derived from other factors
int motorSpeed(bool varied = false) {
  if(awake) {
    intensity = map(knobValue[0], 0, 1023, 0, intensityMax);
    return variedIntensity();
  } else {
    return 0;
  }
}

// vary the target intensity (pwm)
long variedIntensity() {
  // lighten it up
  if (random(3) < 1) {
    return 0;
  }
  return random(50, intensity+1); // add '1' since upper boundary is exclusive
}

// alter ramp time
void randomizeRampTime(int i) {
  int factor = random(int(rampFactor * float(timeScale)));
  // sometimes + sometimes -
  if (random(2) == 0) {
    factor = factor * (-1);
  }
  rampTime[i] = constrain(rampBasis + factor, int(motorRampMin * float(timeScale)), int(motorRampMax * float(timeScale)));
}

void ramp(int i, bool rampUp = false) {
  // rampUp = true --> ramp up
  // rampUp = false --> ramp down
  rampingUp[i] = rampUp;
  randomizeRampTime(i);
  rampTimer[i].setTimeout(rampTime[i]);
  rampTimer[i].restart();
  if (awake) {
    if (rampUp) {
      motor[i].ramp(motorSpeed(), rampTime[i]);
    } else {
      // TODO: explore making the ramp timer shorter for the sustain period
      motor[i].ramp(motorSpeed(), rampTime[i]);
      // Serial.print("Sustain PWM for [");
      // Serial.print(i);
      // Serial.print("]: ");
      // Serial.print(variedIntensity());
      // Serial.print(" for ");
      // Serial.println(rampTime[i]);
    }
  } else {
    // off
    motor[i].ramp(0, rampTime[i]);
  }
}

/* SETUP ---------------------------------------------------- */

void setup() {
  Serial.begin(9600);
  // reseed randomizers w/ unused analog pin
  randomSeed(analogRead(A15));

  // setup digital pins
  for (int i = 0; i < 12; i++) {
    pinMode(ctrlSwitchPin[i], INPUT);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(switchPin[i], INPUT);
  }

  // Read initial values
  for (int i = 0; i < 3; i++) {
    knobValue[i] = analogRead(knobPin[i]);
  }
  for (int i = 0; i < 4; i++) {
    switchValue[i] = digitalRead(switchPin[i]);
  }
  for (int i = 0; i < motorCount; i++) {
    ctrlKnobValue[i] =  analogRead(ctrlKnobPin[i]);
  }
  for (int i = 0; i < motorCount; i++) {
    ctrlSwitchValue[i] = digitalRead(ctrlSwitchPin[i]);
  }

  // set time scales if in manual control mode
  if (switchValue[1] == 1) {
    timeScale = map(knobValue[1], 0, 1023, timeScaleMin, timeScaleMax);
    sleepTime = map(ctrlKnobValue[0], 0, 1023, 6, 180); // 3 minutes;
    awakeTime = map(ctrlKnobValue[1], 0, 1023, 24, 180); // 3 minutes;
    rampBasis = map(ctrlKnobValue[2], 0, 1023, 500, 20000); // half second to 20 seconds;
  }
  Serial.print("SETUP timeScale: ");
  Serial.println(timeScale);
  Serial.print("SETUP sleepTime: ");
  Serial.println(sleepTime);
  Serial.print("SETUP awakeTime: ");
  Serial.println(awakeTime);
  Serial.print("SETUP rampBasis: ");
  Serial.println(rampBasis);

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
  }
  // read ctrlKnobPin 
  for (int i = 0; i < motorCount; i++) {
    ctrlKnobValue[i] =  analogRead(ctrlKnobPin[i]);
  }
  // read ctrlSwitchPin 
  for (int i = 0; i < motorCount; i++) {
    ctrlSwitchValue[i] = digitalRead(ctrlSwitchPin[i]);
  }
  
  // -- Evaluate & Do -- //

  // update time scales if in manual control mode
  if (switchValue[1] == 1) {
    timeScale = map(knobValue[1], 0, 1023, timeScaleMin, timeScaleMax);
    sleepTime = map(ctrlKnobValue[0], 0, 1023, 6, 180); // 3 minutes;
    awakeTime = map(ctrlKnobValue[1], 0, 1023, 24, 180); // 3 minutes;
    rampBasis = map(ctrlKnobValue[2], 0, 1023, 500, 20000); // half second to 20 seconds;
  }

  if(sleepTimer.onExpired()) {
    awake = true;
    awakeTimer.setTimeout(awakeTime * timeScale);
    awakeTimer.restart();

    Serial.print("Waking for ");
    Serial.println(awakeTime * timeScale);
    // output scale for installation reference
    Serial.print("timeScale: ");
    Serial.println(timeScale);
    Serial.print("sleepTime: ");
    Serial.println(sleepTime);
    Serial.print("awakeTime: ");
    Serial.println(awakeTime);
    Serial.print("rampBasis: ");
    Serial.println(rampBasis);
  }

  for (int i = 0; i < motorCount; i++) {
    if(rampTimer[i].onExpired()){
      if (rampingUp[i]) {
        // ramp down if previously ramping up
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
    // output scale for installation reference
    Serial.print("timeScale: ");
    Serial.println(timeScale);
    Serial.print("sleepTime: ");
    Serial.println(sleepTime);
    Serial.print("awakeTime: ");
    Serial.println(awakeTime);
    Serial.print("rampBasis: ");
    Serial.println(rampBasis);
  }

  // update motors
  for (int i = 0; i < motorCount; i++) {
    motor[i].update();
  }

}
