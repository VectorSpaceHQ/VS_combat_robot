#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
//sitck potentiometer input pints.
int potPinUp = A2;
int potPinSide = A3;
int potPinThrot = A0;
int potPinHeading = A1;

//Stick limits from joystick test program
int upBase = 0;
int upMax = 1013;
int upRestMin = 490;
int upRestMax = 525;

int sideBase = 0;
int sideMax = 1013;
int sideRestMin = 490;
int sideRestMax = 530;

int active_connection = 1; // if this gets through, there's an active connection between TX and RX


//variablesd to hold pot values
int upVal;
int sideVal;

//variables to perfom mixing on
int motorLeft;
int motorRight;
int forBack;

//data to send, each will be in range 0-255
int pwmMLforwards;
int pwmMLbackwards;
int pwmMRforwards;
int pwmMRbackwards;
int pwmWeapon;

//radio setups
RF24 radio(9,10);

uint8_t msg[6];
const byte address[6] = "00001";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("STARTING - 00001");
 
  radio.begin();

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  Serial.println("Done SETUP");
}

void loop() {
  //read stick poitions and constrain
  upVal = analogRead(potPinThrot);
  sideVal = analogRead(potPinHeading);
  upVal = 1023 - upVal;
  sideVal = 1023 - sideVal;
  
  upVal=constrain(upVal,upBase,upMax);
  sideVal=constrain(sideVal,sideBase,sideMax);
  
  //clear previous values
  motorLeft = 0;
  motorRight = 0;

  //forwards or backwards variable used to mix the turn sidnals into the forwards and backwards signals
  //set to -1 for backwards to mix the turning values correctly
  forBack=-1;

  //mix motors
  if(upVal<upRestMax&&upVal>upRestMin&&sideVal<sideRestMax&&sideVal>sideRestMin){
    //motor stop contitions
    motorLeft = 0;
    motorRight = 0;
  }else{
    //analyise forwards and back position and mix to motors
    if(upVal>upRestMax){
      //forwards
      motorLeft = motorLeft + map(upVal,upRestMax,upMax,0,255);
      motorRight = motorRight + map(upVal,upRestMax,upMax,0,255);
      forBack=-1;
    }else if(upVal<upRestMin){
      //backwards
      motorLeft = motorLeft - map(upVal,upRestMin,upBase,0,255);
      motorRight = motorRight - map(upVal,upRestMin,upBase,0,255);
      forBack=1;
    }

    //analyise left right positions 
    if(sideVal>sideRestMax){
      //right
      motorLeft = motorLeft + (map(sideVal,sideRestMax,sideMax,0,255)*forBack);
      motorRight = motorRight - (map(sideVal,sideRestMax,sideMax,0,255)*forBack);
    }else if(sideVal<sideRestMin){
      //left
      motorLeft = motorLeft - (map(sideVal,sideRestMin,sideBase,0,255)*forBack);
      motorRight = motorRight + (map(sideVal,sideRestMin,sideBase,0,255)*forBack);
    }

    //constrain mix to plus/minus pwm values
    motorLeft = constrain(motorLeft,-255,255);
    motorRight = constrain(motorRight,-255,255);
    
  }

  //set weapon pwm, unused for now
  pwmWeapon = analogRead(potPinUp);
  pwmWeapon = map(pwmWeapon, 0, 255, 0, 190);



  //set left motor pwm
  if (motorLeft==0){
    pwmMLforwards = 0;
    pwmMLbackwards = 0;
  }else if(motorLeft<0){
    pwmMLforwards = 0;
    pwmMLbackwards = (motorLeft*-1);
  }else if(motorLeft>0){
    pwmMLforwards = motorLeft;
    pwmMLbackwards = 0;
  }

  //set right motor pwm
  if (motorRight==0){
    pwmMRforwards = 0;
    pwmMRbackwards = 0;
  }else if(motorRight<0){
    pwmMRforwards = 0;
    pwmMRbackwards = (motorRight*-1);
  }else if(motorRight>0){
    pwmMRforwards = motorRight;
    pwmMRbackwards = 0;
  }

 //send values and write to serial
  msg[0]=pwmMLforwards;
  msg[1]=pwmMLbackwards;
  msg[2]=pwmMRforwards;
  msg[3]=pwmMRbackwards;
  msg[4]=pwmWeapon;
  msg[5]=active_connection;

  
  Serial.print(pwmMLforwards);
  Serial.print(", ");
  Serial.print(pwmMLbackwards);
  Serial.print(", ");
  Serial.print(pwmMRforwards);
  Serial.print(", ");
  Serial.print(msg[3]);
  Serial.print(", ");
  Serial.print(msg[4]);
  Serial.print(", ");
  Serial.println(msg[5]);
  radio.write(msg,6);

  //delay sometimes needed. can be modified to suit
  delay(10);

}
