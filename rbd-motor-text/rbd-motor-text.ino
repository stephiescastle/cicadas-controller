// Arduino RBD Motor Library v2.1.2 Example - Spin a motor up and down with events.
// https://github.com/alextaujenis/RBD_Motor
// Copyright (c) 2015 Alex Taujenis - MIT License

// specific example https://github.com/alextaujenis/RBD_Motor/issues/5
// basic example  https://github.com/alextaujenis/RBD_Motor/blob/master/examples/spin_up_and_down/spin_up_and_down.ino

#include <RBD_Timer.h> // https://github.com/alextaujenis/RBD_Timer
#include <RBD_Motor.h> // https://github.com/alextaujenis/RBD_Motor

/* VARS & CLASSES -------------------------------------------- */

// Global Pod States
  // time scale
int timeScale = 1;
  // sleep duration TODO: use timescale
unsigned long sleepTime = 4000;
  // sleep timer
RBD::Timer sleepTimer(sleepTime);
  // awake duration TODO: use timescale
unsigned long awakeTime = 12000;
  // awake timer
RBD::Timer awakeTimer(awakeTime);
  // if pod is awake or sleeping
bool awake = false;

// Individual Cicada States
  // motor pins
RBD::Motor motor[] = {(3), (5)};
  // ramp direction for each per motor
bool rampingUp[] = {true, true}; 
  // ramp durations for each motor TODO: use timescale
unsigned long rampTime[] = {1200, 1200};
  // create and set timer for each motor
RBD::Timer rampTimer[] = {(rampTime[0]), rampTime[1]};
  // use in for loops
static const uint8_t motorCount = 2;
  // min motor ramp time TODO: use time scale
int motorRampMin = 700;
  // max motor ramp time TODO: use time scale
int motorRampMax = 1700;

/* UTIL FUNCTIONS ------------------------------------------- */

int motorSpeed() {
  // TODO: use knob to set the value here
  if(awake) {
    return 150;
  } else {
    return 0;
  }
}

// alter ramp time
void randomizeRampTime(int i) {
  int factor = random(76);
  if (random(2) == 0) {
    factor = factor * (-1);
  }
  rampTime[i] = constrain(rampTime[i] + factor * timeScale, motorRampMin * timeScale, motorRampMax * timeScale);
}

void rampUp(int i) {
  rampingUp[i] = true;
  randomizeRampTime(i);
  rampTimer[i].setTimeout(rampTime[i]);
  rampTimer[i].restart();
  // ramp up the motor
  if (awake) {
    motor[i].ramp(motorSpeed(), rampTime[i]);
  }
}

void rampDown(int i) {
  rampingUp[i] = false;
  randomizeRampTime(i);
  rampTimer[i].setTimeout(rampTime[i]);
  rampTimer[i].restart();
  // ramp down the motor
  if (awake) {
    // TODO: instead of off, maybe make this a randomized pwm value
    motor[i].ramp(0, rampTime[i]);
  }
}

/* SETUP ---------------------------------------------------- */

void setup() {
  Serial.begin(9600);
  // TODO: change this pin number to an unused pin
  randomSeed(analogRead(A5));

  // start asleep
  // TODO: can a knob change this though?
  timeScale = map(analogRead(A0), 0, 670, 1, 100);
  Serial.print("TIMESCALE: ");
  Serial.println(map(analogRead(A0), 0, 670, 1, 100));
  // try isExpired instead of onExpired at beginning?
  sleepTimer.setTimeout(sleepTime * timeScale);
  sleepTimer.restart();

}

/* LOOP ----------------------------------------------------- */

void loop() {  
  // Serial.print("Speed: ");
  // Serial.println(motor[0].getSpeed());

  // TODO: scale times
  // awakeTimer = awakeTimer * scale;
  // sleepTimer = sleepTimer * scale;
  // rampTimer[] = rampTime * scale;
  // Serial.println(analogRead(A0));
  timeScale = map(analogRead(A0), 0, 670, 1, 100);
  if(sleepTimer.onExpired()) {
    Serial.print("TIMESCALE: ");
    Serial.println(map(analogRead(A0), 0, 670, 1, 100));
    Serial.print("Waking for ");
    Serial.println(awakeTime * timeScale);

    awake = true;
    awakeTimer.setTimeout(awakeTime * timeScale);
    awakeTimer.restart();

  }

  for (int i = 0; i < motorCount; i++) {
    if(rampTimer[i].onExpired()){
      if (rampingUp[i]) {
        Serial.print(i);
        Serial.println(": ramp down");
        rampDown(i);
      } else {
        Serial.print(i);
        Serial.println(": ramp up");
        rampUp(i);
      }
    }
  }

  if (awakeTimer.onExpired()) {
    Serial.println("awake time expired");
    for (int i = 0; i < motorCount; i++) {
      if (rampingUp[i]) {
        rampDown(i);
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
