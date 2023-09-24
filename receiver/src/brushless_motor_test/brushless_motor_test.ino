/*
  Controls servo position from 0-180 degrees and back
  https://wokwi.com/projects/350037178957431378
  by dlloydev, December 2022.
*/

#include <Servo.h>
Servo myservo = Servo();

const int servoPin = D5; //D5 is esc, D10 is servo

void setup() {  
  arm();

}


void arm() // https://www.helifreak.com/showthread.php?t=412147
{

  myservo.write(servoPin, 20);        // zero throttle
  delay(4000);
  myservo.write(servoPin, 90);        // mid throttle low tone
  delay(2000);
  myservo.write(servoPin, 20);        // set the servo position (degrees)
  delay(200);
}


void loop() {
  for (int pos = 20; pos <= 180; pos++) {  // go from 0-180 degrees
    myservo.write(servoPin, pos);        // set the servo position (degrees)
    delay(15);
  }
  delay(500);
  for (int pos = 180; pos >= 20; pos--) {  // go from 180-0 degrees
    myservo.write(servoPin, pos);        // set the servo position (degrees)
    delay(15);
  }
  delay(500);
}
