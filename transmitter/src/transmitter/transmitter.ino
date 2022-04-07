#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
//sitck potentiometer input pints.
int weapon_pin = 6;
int leftX = A2;
int leftY = A3;
int rightY = A0;
int rightX = A1;
int leftVal;
int rightVal;
int LEDPin;

int leftBase = 512;
int leftRestMin = leftBase - 20;
int leftRestMax = leftBase + 20;
int rightBase = 512;
int rightRestMin = rightBase - 20;
int rightRestMax = rightBase + 20;

int active_connection = 1; // if this gets through, there's an active connection between TX and RX

//data to send, each will be in range 0-255
int rightForward;
int rightBackward;
int leftForward;
int leftBackward;
int pwmWeapon;
bool weapon_state;

//radio setups
RF24 radio(9, 10);

uint8_t msg[6];
const byte address[6] = "11001";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.print("STARTING - ");
  delay(1000);


  pinMode(weapon_pin, INPUT_PULLUP);

  radio.begin();

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  Serial.println("Done SETUP");
}

void loop() {
  //read stick poitions and constrain

  leftVal = analogRead(leftY);
  rightVal = analogRead(rightY);
  rightVal = 1023 - rightVal;

  if (leftVal < leftRestMax && leftVal > leftRestMin) {
    leftForward = 0;
    leftBackward = 0;
  }
  else {
    if (leftVal > leftRestMax) {
      leftForward = map(leftVal, leftRestMax, 1023, 0, 255);
    }
    else if (leftVal < leftRestMin) {
      leftBackward = map(leftVal, leftRestMin, 0, 0, 255);
    }
  }

  if (rightVal < rightRestMax && rightVal > rightRestMin) {
    rightForward = 0;
    rightBackward = 0;
  }
  else {
    if (rightVal > rightRestMax) {
      rightForward = map(rightVal, rightRestMax, 1023, 0, 255);
    }
    else if (rightVal < rightRestMin) {
      rightBackward = map(rightVal, rightRestMin, 0, 0, 255);
    }
  }

  //set weapon pwm, unused for now
  weapon_state = digitalRead(weapon_pin);
  pwmWeapon = map(weapon_state, 0, 1, 190, 0);

  //send values and write to serial
  msg[0] = leftForward;
  msg[1] = leftBackward;
  msg[2] = rightForward;
  msg[3] = rightBackward;
  msg[4] = pwmWeapon;
  msg[5] = active_connection;

  Serial.print(leftForward);
  Serial.print(", ");
  Serial.print(leftBackward);
  Serial.print(", ");
  Serial.print(rightForward);
  Serial.print(", ");
  Serial.print(rightBackward);
  Serial.print(", ");
  Serial.print(pwmWeapon);
  Serial.print(", ");
  Serial.println(msg[5]);

  radio.write(msg, 6);

  //delay sometimes needed. can be modified to suit
  delay(10);

}
