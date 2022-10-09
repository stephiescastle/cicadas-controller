// Arduino RBD Motor Library v2.1.2 Example - Spin a motor up and down with events.
// https://github.com/alextaujenis/RBD_Motor
// Copyright (c) 2015 Alex Taujenis - MIT License

// specific example https://github.com/alextaujenis/RBD_Motor/issues/5
// basic example  https://github.com/alextaujenis/RBD_Motor/blob/master/examples/spin_up_and_down/spin_up_and_down.ino

#include <RBD_Timer.h> // https://github.com/alextaujenis/RBD_Timer
#include <RBD_Motor.h> // https://github.com/alextaujenis/RBD_Motor

/* VARS & CLASSES -------------------------------------------- */

// ---- Global Brood States ---- //
// if group is awake or sleeping
bool awake = false;
// time scale
long timeScale = 1000;
// min timeScale (ms);
int timeScaleMin = 500;
// max timeScale (ms);
int timeScaleMax = 2000;
// sleep duration (s)
unsigned long sleepTime = 6;
// awake duration  (s)
unsigned long awakeTime = 20;
// sleep timer
RBD::Timer sleepTimer(sleepTime*timeScale);
// awake timer
RBD::Timer awakeTimer(awakeTime*timeScale);

// ---- Individual Cicada States ---- //
// number of motors in arrays
static const uint8_t motorCount = 2;
// motor pins
RBD::Motor motor[] = {(3), (5)};
// ramp direction for each per motor
bool rampingUp[] = {true, true}; 
// ramp durations for each motor (ms)
unsigned long rampTime[] = {3000, 3000};
// create and set timer for each motor
RBD::Timer rampTimer[] = {(rampTime[0]), rampTime[1]};
// min motor ramp time (s)
float motorRampMin = 0.7;
// max motor ramp time (s)
float motorRampMax = 1.7;
// randomization range (s)
float rampFactor = 0.076;

// ---- Knob assignments ---- //

// TODO: alternate: use switch on main board to set mode for knobs
// alt mode would be timing changes, like ramp time basis, and awake time or sleep time
// wish i had three knobs!
int timeScaleKnob = A0;
int intensityKnob = A1;
int intensity = 150;
int intensityMax = 255;

/* UTIL FUNCTIONS ------------------------------------------- */

int motorSpeed() {
  // TODO: use knob to set the value here
  if(awake) {
    intensity = map(analogRead(intensityKnob), 0, 670, 0, intensityMax);
    return intensity;
  } else {
    return 0;
  }
}

// alter ramp time
void randomizeRampTime(int i) {
  int factor = random(int(rampFactor * float(timeScale)));
  if (random(2) == 0) {
    factor = factor * (-1);
  }
  rampTime[i] = constrain(rampTime[i] + factor, int(motorRampMin * float(timeScale)), int(motorRampMax * float(timeScale)));
}

void ramp(int i, bool up) {
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
      // TODO: instead of 0 or off, maybe make this a randomized pwm value
      motor[i].ramp(0, rampTime[i]);
    }
  }
}

// void rampUp(int i) {
//   rampingUp[i] = true;
//   randomizeRampTime(i);
//   rampTimer[i].setTimeout(rampTime[i]);
//   rampTimer[i].restart();
//   if (awake) {
//     motor[i].ramp(motorSpeed(), rampTime[i]);
//   }
// }

// void rampDown(int i) {
//   rampingUp[i] = false;
//   randomizeRampTime(i);
//   rampTimer[i].setTimeout(rampTime[i]);
//   rampTimer[i].restart();
//   if (awake) {
//     // TODO: instead of 0 or off, maybe make this a randomized pwm value
//     motor[i].ramp(0, rampTime[i]);
//   }
// }

/* SETUP ---------------------------------------------------- */

void setup() {
  Serial.begin(9600);
  // TODO: change this pin number to an unused pin
  randomSeed(analogRead(A5));

  // set timeScale
  timeScale = map(analogRead(timeScaleKnob), 0, 670, timeScaleMin, timeScaleMax);
  Serial.print("TIMESCALE SETUP: ");
  Serial.println(timeScale);

  // start asleep
  sleepTimer.setTimeout(sleepTime * timeScale);
  sleepTimer.restart();

}

/* LOOP ----------------------------------------------------- */

void loop() {  
  
  // update timeScale
  timeScale = map(analogRead(timeScaleKnob), 0, 670, timeScaleMin, timeScaleMax);

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
        ramp(i, false);
      } else {
        // ramp up
        ramp(i, true);
      }
    }
  }

  if (awakeTimer.onExpired()) {
    // ramp down anything in progress
    for (int i = 0; i < motorCount; i++) {
      if (rampingUp[i]) {
        ramp(i, false);
      }
    }
    // go to sleep
    awake = false;
    sleepTimer.setTimeout(sleepTime * timeScale);
    sleepTimer.restart();
    Serial.print("Sleeping for ");
    Serial.println(sleepTime * timeScale);
  }

  for (int i = 0; i < motorCount; i++) {
    motor[i].update();
  }

}
