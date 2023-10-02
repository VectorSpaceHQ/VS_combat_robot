#include <Preferences.h>
Preferences preferences; // https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/

const int leftJoyPin = A1;
const int rightJoyPin = A2; // D3 is actual but not working
int leftVal;
int rightVal;
uint16_t leftRestVal;
uint16_t leftMin; 
uint16_t leftMax; 
uint16_t rightRestVal;
uint16_t rightMin; 
uint16_t rightMax;


void loadValues(){
  leftMax = preferences.getUShort("leftMax",4095);
  leftMin = preferences.getUShort("leftMin",0);
  leftRestVal = preferences.getUShort("leftRestVal",2000);
  rightMax = preferences.getUShort("rightMax",4095);
  rightMin = preferences.getUShort("rightMin",0);
  rightRestVal = preferences.getUShort("rightRestVal",2000);

  Serial.println("Values loaded from EEPROM:");
  Serial.print(leftMin);
  Serial.print(", ");
  Serial.print(leftRestVal);
  Serial.print(", ");
  Serial.println(leftMax);
  delay(100);
}


void saveValues(){
  Serial.println("Saving values to EEPROM");
  preferences.putUShort("leftMax", leftMax);
  preferences.putUShort("leftMin", leftMin);
  preferences.putUShort("leftRestVal", leftRestVal);
  preferences.end();
}


void setup() {
  preferences.begin("joystick", false);
  Serial.begin(115200);
  
  pinMode(leftJoyPin, INPUT_PULLUP);
  pinMode(rightJoyPin, INPUT_PULLUP);

  delay(1000);
  Serial.println("Starting up");
  loadValues();
  delay(2000);


  Serial.println("Hold LEFT joystick forward");
  while((4095- analogRead(leftJoyPin)) < 3500){
    delay(10);
  }
  Serial.println("3");
  delay(1000);
  Serial.println("2");
  delay(1000);
  Serial.println("1");
  delay(1000);
  
  Serial.print("Left joystick MAX value = ");
  for(int i=0; i<10; i++)
  {
    leftMax += (4095 - analogRead(leftJoyPin)) / 10; // upside down
    delay(1);
  }
  Serial.println(leftMax);
  

  Serial.println("Hold LEFT joystick backward");
  while((4095-analogRead(leftJoyPin)) > 500){
     delay(10);
  }
  Serial.println("3");
  delay(1000);
  Serial.println("2");
  delay(1000);
  Serial.println("1");
  delay(1000);
  for(int i=0; i<10; i++)
  {
    leftMin += (4095 - analogRead(leftJoyPin)) / 10; // upside down
    delay(1);
  }
  Serial.print("Left joystick MIN value = ");
  Serial.println(leftMin);
  

  Serial.println("Let go of LEFT joystick");
  while((4095-analogRead(leftJoyPin)) < 1500 or (4095-analogRead(leftJoyPin)) > 3000){
     delay(10);
  }
  Serial.println("3");
  delay(1000);
  Serial.println("2");
  delay(1000);
  Serial.println("1");
  delay(1000);
  for(int i=0; i<10; i++)
  {
    leftRestVal += (4095 - analogRead(leftJoyPin)) / 10; // upside down
    delay(1);
  }
  Serial.print("Left joystick Rest value = ");
  Serial.println(leftRestVal);


  saveValues();
}



void loop(){
  Serial.print(leftMin);
  Serial.print(", ");
  Serial.print(leftRestVal);
  Serial.print(", ");
  Serial.println(leftMax);
  delay(1000);
}
