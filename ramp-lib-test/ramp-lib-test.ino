#include <Ramp.h>                           // include library
ramp myRamp;                                // new ramp object (ramp<unsigned char> by default)
int target = 255;
void setup() {
  Serial.begin(9600);                       // begin Serial communication
  
  Serial.print("Value start at: ");         //
  Serial.println(myRamp.getValue());        // accessing start value
  Serial.println("Starting interpolation"); // 
  myRamp.go(target, 10000);               // start interpolation (value to go to, duration)
}

void loop() {
  Serial.print("Actual value is: ");
  Serial.println(myRamp.update());
  
  analogWrite(3, myRamp.getValue());
  if (myRamp.getValue() == 0) {
    target = 255;
    // myRamp.go(target, 10000);
  }
  if (myRamp.getValue() == 255) {
    target = 0;
    myRamp.go(target, 10000);
  }
  // myRamp.go(target, 10000);
  
  // analogWrite(3, myRamp.getValue());
}
