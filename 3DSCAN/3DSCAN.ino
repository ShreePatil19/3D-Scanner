

/*
 * This code contains the follow functions:
 * - void setup(): initializes Serial port, SD card
 * - void loop(): main loop
 * - double readAnalogSensor(): calculates sensed distances in cm.  Sensed values calculated as an average of noSamples consecutive analogRead() calls to eliminate noise
 * - void writeToSD(): Writes sensed distance in cm to SD card file specified by filename variable
 * - void readFromSD(): Prints out contents of SD card file specified by filename variable to Serial
 */
 
 /* 
 * Pinout:
 * SD card attached to SPI bus as follows:
 * - MOSI - pin 16
 * - MISO - pin 14
 * - CLK - pin 15
 * - CS - pin 10
 * 
 * IR Sensor (SHARP GP2Y0A51SK0F: 2-15cm, 5V) attached to microcontroller as follows:
 * - Sense - A3
 * 
 * Turntable stepper motor driver board:
 * - STEP - pin 2
 * - DIR - pin 3 
 * - MS1 - pin 4
 * - MS2 - pin 5
 * - Enable - pin 6
 * 
 * Z-Axis stepper motor driver board:
 * - STEP - pin 7
 * - DIR - pin 8
 * - MS1 - pin 9
 * - MS2 - pin 18 (A0 on Arduino Pro Micro silkscreen)
 * - ENABLE - pin 19 (A1 on Arduino Pro Micro silkscreen)
 */

#include <SPI.h>
#include <SD.h>
//
//File scannerValues;
//String filename="scn000.txt";
//int csPin=10;
int sensePin=A5;
SharpIR mySensor = SharpIR(sensePin, model);


int tStep=2;
int tDir=3;
int tMS1=11;
int tMS2=10;
int tMS3=9;

int zStep=5;
int zDir=4;
//int zMS1=12;
//int zMS2=13;
//int zEnable=19;


void setup() 
{ 

  //Define stepper pins as digital output pins
  pinMode(tStep,OUTPUT);
  pinMode(tDir,OUTPUT);
  pinMode(tMS1,OUTPUT);
  pinMode(tMS2,OUTPUT);
  pinMode(tMS3,OUTPUT);
  pinMode(zStep,OUTPUT);
  pinMode(zDir,OUTPUT);
//  pinMode(zMS1,OUTPUT);
//  pinMode(zMS2,OUTPUT);
//  pinMode(zEnable,OUTPUT);
  mySensor = SharpIR(sensePin, model);

  //Set microstepping mode for stepper driver boards.  Using 1.8 deg motor angle (200 steps/rev) NEMA 17 motors (12V)

/*
  // Theta motor: 1/2 step micro stepping (MS1 High, MS2 Low) = 0.9 deg/step (400 steps/rev)
  digitalWrite(tMS1,HIGH);
  digitalWrite(tMS2,LOW);
*/
  
  
  // Theta motor: no micro stepping (MS1 Low, MS2 Low) = 1.8 deg/step (200 steps/rev)
  digitalWrite(tMS1,LOW);
  digitalWrite(tMS2,LOW);
  
  
  // Z motor: no micro stepping (MS1 Low, MS2 Low) = 1.8 deg/step (200 steps/rev) --> (200 steps/1cm, i.e. 200 steps/10 mm).  Therefore 0.05mm linear motion/step.
  digitalWrite(zMS1,LOW);
  digitalWrite(zMS2,LOW);

  //Set rotation direction of motors
  //digitalWrite(tDir,HIGH);
  //digitalWrite(zDir,LOW);
  //delay(100);

}


void loop() 
{

  int vertDistance=12; //Total desired z-axis travel
  int noZSteps=20; //No of z-steps per rotation.  Distance = noSteps*0.05mm/step
  int zCounts=(200/1*vertDistance)/noZSteps; //Total number of zCounts until z-axis returns home
  //int thetaCounts=400;
  int thetaCounts=200;
  

  // Scan object
  digitalWrite(zDir,LOW); 
  for (int j=0; j<zCounts; j++) //Rotate z axis loop
  {
    for (int i=0; i<thetaCounts; i++)   //Rotate theta motor for one revolution, read sensor and store
    {
      rotateMotor(tStep, 1); //Rotate theta motor one step
      delay(200);
      double senseDistance=0; //Reset senseDistanceVariable;
      senseDistance=readAnalogSensor(); //Read IR sensor, calculate distance
      Serial.println(senseDistance)
    }
  
    rotateMotor(zStep, noZSteps); //Move z carriage up one step
    delay(1000);
//    Serial.println(senseDistance) //Write dummy value to SD for later parsing
  }

  // Scan complete.  Rotate z-axis back to home and pause.
  digitalWrite(zDir,HIGH);
  delay(10);  
  for (int j=0; j<zCounts; j++)
  {
    rotateMotor(zStep, noZSteps);
    delay(10);
  }

  for (int k=0; k<3600; k++) //Pause for one hour (3600 seconds), i.e. freeze until power off because scan is complete.
  {
    delay(1000); 
  }

  //readFromSD(); //Debug - Read from SD 
}

void rotateMotor(int pinNo, int steps)
{
  
  for (int i=0; i<steps; i++)
  {
    digitalWrite(pinNo, LOW); //LOW to HIGH changes creates the
    delay(1);
    digitalWrite(pinNo, HIGH); //"Rising Edge" so that the EasyDriver knows when to step.
    delay(1);
    //delayMicroseconds(500); // Delay so that motor has time to move
    //delay(100); // Delay so that motor has time to move
  }
}

double readAnalogSensor()
{

  //int noSamples=10;  
  int noSamples=100;
  int sumOfSamples=0;

  int senseValue=0;
  double senseDistance=0;
  
  for (int i=0; i<noSamples; i++)
  {
    senseValue=analogRead(sensePin); //Perform analogRead
    delay(2); //Delay to let analogPin settle -- not sure if necessary
    sumOfSamples=sumOfSamples+senseValue; //Running sum of sensed distances
  }
  
  senseValue=sumOfSamples/noSamples; //Calculate mean
  senseDistance=senseValue; //Convert to double
  senseDistance=mapDouble(senseDistance,0.0,1023.0,0.0,5.0); //Convert analog pin reading to voltage
  //Serial.print("Voltage: ");     //Debug
  //Serial.println(senseDistance);   //Debug
  //Serial.print(" | "); //Debug
  senseDistance=13*pow(senseValue, -1); //Convert voltage to distance in cm via cubic fit of Sharp sensor datasheet calibration
  //Print to Serial - Debug
  Serial.print("Distance: ");    //Debug
  Serial.println(senseDistance); //Debug
  Serial.print(senseValue);
  Serial.print("   |   ");
  Serial.println(senseDistance);
  return senseDistance;
}



double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
