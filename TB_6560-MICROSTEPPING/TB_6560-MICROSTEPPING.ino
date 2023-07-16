/* Example sketch to control a stepper motor with TB6560 stepper motor driver, AccelStepper library and Arduino: acceleration and deceleration. More info: https://www.makerguides.com */

// Include the AccelStepper library:
#include <AccelStepper.h>

// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1

// Create a new instance of the AccelStepper class:
AccelStepper myStepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {
  myStepper.setMaxSpeed(5);
  myStepper.setAcceleration(5);
  myStepper.setSpeed(5);
  myStepper.moveTo(100);
} 
void loop() {
  if (myStepper.distanceToGo() == 0) 
    myStepper.moveTo(-myStepper.currentPosition());

  myStepper.run();
}
