// Arduino RBD Motor Library v2.1.2 Example - Spin a motor up and down with events.
// https://github.com/alextaujenis/RBD_Motor
// Copyright (c) 2015 Alex Taujenis - MIT License

// specific example https://github.com/alextaujenis/RBD_Motor/issues/5
// basic example  https://github.com/alextaujenis/RBD_Motor/blob/master/examples/spin_up_and_down/spin_up_and_down.ino

#include <RBD_Timer.h> // https://github.com/alextaujenis/RBD_Timer
#include <RBD_Motor.h> // https://github.com/alextaujenis/RBD_Motor

RBD::Motor motor(3); // pwm pin
RBD::Timer sleepTimer; // track the sleepTime
RBD::Timer emergeTimer; // track the emergeTime
RBD::Timer ramp_timer; // track the ramp up and down time // TODO: will need for each motor []

unsigned long emergeTime = 10000; // ms
unsigned long sleepTime = 13000; // ms
bool is_emerged = false;
unsigned long motor_ramp_time = 1000; // TODO: will need for each motor []
bool ramping_up = true; // TODO: will need for each motor []

void setup() {
  Serial.begin(9600);
   emergeTimer.setTimeout(emergeTime);
   sleepTimer.setTimeout(sleepTime);
   ramp_timer.setTimeout(motor_ramp_time);
}

void loop() {  
  Serial.print("Speed: ");
  Serial.println(motor.getSpeed());
  
//  if (sleepTimer.onExpired()) {
//    is_emerged = true;
//    
//  }

  if(sleepTimer.onExpired()) {
    // set a flag to disable the button during the emergeTime
    is_emerged = true;
    Serial.println("SLEEP time expired");
    
    // update the emergeTime timer
    emergeTimer.setTimeout(emergeTime);
    
    // start the emergeTime timer
    emergeTimer.restart();
    
    // start the rampup timer
    ramping_up = true;
    ramp_timer.restart();
    
    // ramp up the motor
    motor.ramp(motorSpeed(), motor_ramp_time);
    
  }
  if(ramp_timer.onExpired()){
    // start the rampdown timer
    if (ramping_up) {
       ramping_up = false;
       ramp_timer.restart();
       // ramp down the motor
       if (is_emerged) {
         motor.ramp(0, motor_ramp_time);
       }
    } else {
      ramping_up = true;
      ramp_timer.restart();
      // ramp up the motor
      if (is_emerged) {
        motor.ramp(motorSpeed(), motor_ramp_time);
      }
    }
  }

  if (emergeTimer.onExpired()) {
    is_emerged = false;
    Serial.println("emerge time expired");
    sleepTimer.restart();
  }

motor.update();
//  // RAMP LIBRARY EXAMPLE
//  myRamp.update());
//  
//  analogWrite(3, myRamp.getValue());
//  if (myRamp.getValue() == 0) {
//    target = 255;
//    // myRamp.go(target, 10000);
//  }
//  if (myRamp.getValue() == 255) {
//    target = 0;
//    myRamp.go(target, 10000);
//  }

}

int motorSpeed() {
  if(is_emerged) {
    return 150;
  }
  else {
    return 0;
  }
}
