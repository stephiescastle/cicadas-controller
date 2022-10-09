// Arduino RBD Motor Library v2.1.2 Example - Spin a motor up and down with events.
// https://github.com/alextaujenis/RBD_Motor
// Copyright (c) 2015 Alex Taujenis - MIT License

// specific example https://github.com/alextaujenis/RBD_Motor/issues/5
// basic example  https://github.com/alextaujenis/RBD_Motor/blob/master/examples/spin_up_and_down/spin_up_and_down.ino

#include <RBD_Timer.h> // https://github.com/alextaujenis/RBD_Timer
#include <RBD_Motor.h> // https://github.com/alextaujenis/RBD_Motor

/* VARS & CLASSES -------------------------------------------- */

// Global Pod States
  // sleep duration
unsigned long sleepTime = 3000;
  // sleep timer
RBD::Timer sleepTimer(sleepTime);
  // awake duration
unsigned long awakeTime = 10000;
  // awake timer
RBD::Timer awakeTimer(awakeTime);
  // if pod is awake or sleeping
bool awake = false;

// Individual Cicada States
  // motor pins
RBD::Motor motor[] = {(3), (5)};
  // ramp direction for each per motor
bool rampingUp[] = {true, true}; 
  // ramp durations for each motor
unsigned long rampTime[] = {1000, 1000};
  // create and set timer for each motor
RBD::Timer rampTimer[] = {(rampTime[0]), rampTime[1]};
  // easy var for array lengths
int motorCount = 2;

/* UTIL FUNCTIONS ------------------------------------------- */

int motorSpeed() {
  // TODO: use knob to set the value here
  if(awake) {
    return 170;
  } else {
    return 0;
  }
}

/* SETUP ---------------------------------------------------- */

void setup() {
  Serial.begin(9600);
  // start asleep
  // TODO: can a knob change this though?
  sleepTimer.setTimeout(sleepTime);
  sleepTimer.restart();
}

/* LOOP ----------------------------------------------------- */

void loop() {  
  // Serial.print("Speed: ");
  // Serial.println(motor[0].getSpeed());

  // scale times
  // awakeTimer = awakeTimer * scale;
  // sleepTimer = sleepTimer * scale;
  // rampTimer[] = rampTime * scale;

  if(sleepTimer.onExpired()) {
    Serial.println("SLEEP time expired");

    awake = true;
    
    // update the awakeTime timer
    awakeTimer.setTimeout(awakeTime);
    
    // start the awakeTime timer
    awakeTimer.restart();
    
    // start the rampUp timer for each motor
    for (int i = 0; i < motorCount; i++) {
      rampingUp[i] = true;
      rampTimer[i].setTimeout(rampTime[i]);
      rampTimer[i].restart();

      // ramp up the motor
      motor[i].ramp(motorSpeed(), rampTime[i]);
    }    
  }
  for (int i = 0; i < motorCount; i++) {
    if(rampTimer[i].onExpired()){
      // start the rampdown timer
      if (rampingUp[i]) {
        rampingUp[i] = false;
        rampTimer[i].restart();
        // ramp down the motor
        if (awake) {
          motor[i].ramp(0, rampTime[i]);
        }
      } else {
        rampingUp[i] = true;
        rampTimer[i].restart();
        // ramp up the motor
        if (awake) {
          motor[i].ramp(motorSpeed(), rampTime[i]);
        }
      }
    }
  }

  if (awakeTimer.onExpired()) {
    Serial.println("awake time expired");
    for (int i = 0; i < motorCount; i++) {
      // make sure it turns off
      rampingUp[i] = false;
      rampTimer[i].restart();
      motor[i].ramp(0, rampTime[i]);
    }
    // go to sleep
    awake = false;
    sleepTimer.setTimeout(sleepTime);
    sleepTimer.restart();
  }

  for (int i = 0; i < motorCount; i++) {
    motor[i].update();
  }

}
