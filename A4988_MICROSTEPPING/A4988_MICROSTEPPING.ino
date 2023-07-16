#include <AccelStepper.h>
// Define pin connections

const int dirPin = 5;
const int stepPin = 4;

// Define motor interface type
#define motorInterfaceType 1
AccelStepper myStepper(motorInterfaceType, stepPin, dirPin);
void setup() {
  myStepper.setMaxSpeed(1000);
  myStepper.setAcceleration(40);
  myStepper.setSpeed(100);
  myStepper.moveTo(150);
} 
void loop() {
//  if (myStepper.distanceToGo() == 0) 
//    myStepper.moveTo(-myStepper.currentPosition());

  myStepper.run();
}
