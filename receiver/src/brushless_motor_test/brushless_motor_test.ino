/*
  Controls servo position from 0-180 degrees and back
  https://wokwi.com/projects/350037178957431378
  by dlloydev, December 2022.
*/

#include <Servo.h>
Servo my_brushless_motor = Servo();

const int motorPin = D5; //D5 is esc, D10 is servo
const int minThrottle = 15;
const int maxThrottle = 24;

void setup() {  
  Serial.begin(115200);
  arm();

}


void arm() // https://www.helifreak.com/showthread.php?t=412147
{
  my_brushless_motor.write(motorPin, minThrottle);        // zero throttle
  delay(4000);
  my_brushless_motor.write(motorPin, 40);        // mid throttle low tone
  delay(2000);
  my_brushless_motor.write(motorPin, minThrottle);        // set the servo position (degrees)
  delay(200);
}


void loop() {
  for (int pos = 20; pos <= 180; pos++) { 
    my_brushless_motor.write(motorPin, pos);   
    Serial.println(pos);     
    delay(15);
  }
  delay(500);
  for (int pos = 180; pos >= 20; pos--) {  // go from 180-0 degrees
    my_brushless_motor.write(motorPin, pos);        // set the servo position (degrees)
     Serial.println(pos); 
    delay(15);
  }
  delay(500);
}
