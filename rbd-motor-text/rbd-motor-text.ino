// Arduino RBD Motor Library v2.1.2 Example - Spin a motor up and down with events.
// https://github.com/alextaujenis/RBD_Motor
// Copyright (c) 2015 Alex Taujenis - MIT License

#include <RBD_Timer.h> // https://github.com/alextaujenis/RBD_Timer
#include <RBD_Motor.h> // https://github.com/alextaujenis/RBD_Motor

RBD::Motor motor(3); // pwm pin
RBD::Timer runtime_timer; // track the runtime
RBD::Timer ramp_timer; // track the ramp up and down time
bool ramping_up = true;
unsigned long runtime = 10000;
bool is_running = false;
unsigned long motor_ramp_time = 1000;

void setup() {
  // motor.rampDown(100);
  Serial.begin(9600);
  runtime_timer.setTimeout(runtime);
  ramp_timer.setTimeout(motor_ramp_time);
}

void loop() {
  // motor.update();
  
  Serial.print("Speed: ");
  Serial.println(motor.getSpeed());
//  
//  if (motor.isOff()) {
//    motor.setSpeed(100);
//    Serial.println("off bitches");
//    motor.rampUp(13000);
//  } else if(motor.isOn()) {
//    motor.rampDown(3000);
//  }

  if(!is_running) {
    // set a flag to disable the button during the runtime
    is_running = true;
    // update the runtime timer
    runtime_timer.setTimeout(runtime);
    // start the runtime timer
    runtime_timer.restart();
    // start the rampup timer
    ramping_up = true;
    ramp_timer.restart();
    // ramp up the motor
    motor.ramp(motorSpeed(), motor_ramp_time);
    // start playing the MP3
    // mp3.start();
  }
   if(ramp_timer.onExpired()){
    // start the rampdown timer
    if (ramping_up) {
       ramping_up = false;
       ramp_timer.restart();
       // ramp down the motor
       motor.ramp(0, motor_ramp_time);
    } else {
      ramping_up = true;
    ramp_timer.restart();
    // ramp up the motor
    motor.ramp(motorSpeed(), motor_ramp_time);
    }

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
  if(is_running) {
    return 150;
  }
  else {
    return 0;
  }
}
