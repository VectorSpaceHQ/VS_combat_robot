#include <SoftPWM.h>
#include <SoftPWM_timer.h>

#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <Servo.h>

Servo ESC;     // create servo object to control the ESC
RF24 radio(9,10);
const byte address[6] = "11001";
uint8_t msg[6]; 
/*
int leftFor = 3;
int leftBack = 4;
int rightFor = 5;
int rightBack = 6;
*/

int leftFor = 5;
int leftBack = 4;
int rightFor = 6;
int rightBack = 3;


int LEDPin = 7;
int weapon_pin = 2;
int active_connection = 0;

int signalLostCounter;

int weapon;
int dataLF;
int dataLB;
int dataRF;
int dataRB;

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  Serial.println("SETUP - ");
  
  pinMode(LEDPin, OUTPUT);

  SoftPWMBegin();
  SoftPWMSet(leftFor, 0);
  SoftPWMSet(leftBack, 0);
  SoftPWMSet(rightFor, 0);
  SoftPWMSet(rightBack, 0);

  delay(1000);

  radio.begin();

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
  


  ESC.attach(weapon_pin,1000,2000); // (pin, min pulse width, max pulse width in microseconds) 
  // holding value at 0 for 2 sec arms ESC
  ESC.write(10);
  delay(4000);

  Serial.println("DONE SETUP");

}

void loop() {
  if(radio.available()){
    radio.read(msg,6);
    dataLF=msg[0];
    dataLB=msg[1];
    dataRF=msg[2];
    dataRB=msg[3];
    weapon=msg[4];
    active_connection = msg[5];
  }
  else{
    active_connection = 0;
  }
    
  //if signal from the radio move the bot, otherwise fail to an "all off" condition
  if (active_connection==1){

    if((dataLF>0&&dataLB>0)||(dataRF>0&&dataRB>0)){
      //error condition (attempting to put pwm signal into both forwards and backwards at the same time)
      Serial.println("BOTH ABOVE ERROR");
      SoftPWMSet(leftFor, 0);
      SoftPWMSet(leftBack, 0);
      SoftPWMSet(rightFor, 0);
      SoftPWMSet(rightBack, 0);
      ESC.write(0);    // Send the signal to the ESC


      
    }else{
      Serial.print("good signal, ");
            digitalWrite(LEDPin, HIGH);
      signalLostCounter = 0;
      weapon = constrain(weapon, 0, 180);
      Serial.print(dataLF);
      Serial.print(", ");
      Serial.print(dataLB);
      Serial.print(", ");
      Serial.print(dataRF);
      Serial.print(", ");
      Serial.print(dataRB);
      Serial.print(", ");
      Serial.print(weapon);
      Serial.print(", ");
      Serial.println(active_connection);
      SoftPWMSet(leftFor, 255-dataLF);
      SoftPWMSet(leftBack, 255-dataLB);
      //SoftPWMSet(leftBack, 255);
      //SoftPWMSet(leftFor, 0);
      SoftPWMSet(rightFor, dataRF);
      SoftPWMSet(rightBack, dataRB);
      ESC.write(weapon);    // Send the signal to the ESC. 180 max
    
    }
    
  }else{
    Serial.println("No SIGNAL. Too slow.");
    //No signal from radio, this can happen if the radio is transmitting too slowly
    //either reduce the delay in the tx code or increase the delay below
    signalLostCounter++;
    SoftPWMSet(leftFor, 0);
    SoftPWMSet(leftBack, 0);
    SoftPWMSet(rightFor, 0);
    SoftPWMSet(rightBack, 0);
    if (signalLostCounter >  2){
      ESC.write(0);    // Send the signal to the ESC
      digitalWrite(LEDPin, LOW);
    }
    

    digitalWrite(LEDPin, HIGH);
    delay(20);
    digitalWrite(LEDPin, LOW);
    delay(100);

  }

  //delay for sinking with the tx, adjust as needed
  delay(20);

}
