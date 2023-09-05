#include <Wire.h>
#include <U8g2lib.h>

#include <esp_now.h>
#include <WiFi.h>

#include "screen_icons.h"

// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = {0x10, 0x91, 0xA8, 0x03, 0xD1, 0x18};
//10:91:A8:03:D1:18

typedef struct struct_message {
    int leftJoy;
    int rightJoy;
    bool weapon;
} struct_message;
struct_message myData;

//LCD screen 
U8G2_SSD1306_128X64_NONAME_F_HW_I2C screen(U8G2_R0);

// Joystick variables
const int leftJoyPin = D1;
const int rightJoyPin = D0;
int leftVal;
int rightVal;
int deadband = 20;
int leftRestVal = 2300;
int leftRestMin = leftRestVal - deadband;
int leftRestMax = leftRestVal + deadband;
int rightRestVal = 2420;
int rightRestMin = rightRestVal - deadband;
int rightRestMax = rightRestVal + deadband;

int statusLED1 = D2;
int commsLED = D3;
int pwrWarningLED = D4;


void setup(){
    Serial.begin(115200);

    #if defined ARDUINO_ESP32_DEV  
      Serial.println("ARDUINO_ESP32_DEV");  
    #elif defined ARDUINO_ESP32S2_DEV  
      Serial.println("ARDUINO_ESP32S2_DEV");  
    #elif defined ARDUINO_ESP32C3_DEV  
      Serial.println("ARDUINO_ESP32C3_DEV");  
    #endif  


    Serial.println("Starting up");
    // blink indicator light on controller to show life
    // establish bluetooth connection
    // blink indicator light on bot to show life

    setup_screen();
}


void read_jsticks(){
  int Lcorrection = leftRestVal - 2048;
  int Rcorrection = rightRestVal - 2048;
  leftVal = analogRead(leftJoyPin) - Lcorrection;
  rightVal = analogRead(rightJoyPin) - Rcorrection;
  rightVal = 4095 - rightVal; // installed upside down
  
  
  myData.leftJoy = leftVal;
  myData.rightJoy = rightVal;
}


void read_weapon(){
    myData.weapon = true;
}


void send_data(){
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    if (result == ESP_OK) {
        Serial.println("Sent with success");
    }
    else {
        Serial.println("Error sending the data");
    }
}


void debug_output(){
  Serial.print("Joysticks: ");
  Serial.print(myData.leftJoy);
  Serial.print(", ");
  Serial.println(myData.rightJoy);
}


void check_voltage(){
  Serial.print("Controller voltage Normal");
}
/*
void status_report(){
  check_antenna();
  check_message();
  check_voltage();
  report_failures();
}*/


void loop(){
    read_jsticks();
    read_weapon();
    //send_data();
    update_screen();
    debug_output();
}


void setup_screen(){
  screen.begin();
}

void update_screen(){
  screen.clearBuffer();

  //LEFT sidebar for controller data
  screen.drawBox(16,0,2,64);
  screen.drawXBMP(2,2,12,7,screen_icon_controller);

  //RIGHT sidebar for robot data
  screen.drawBox(109,0,2,64);
  screen.drawXBMP(113,2,12,7,screen_icon_robot);

  screen.sendBuffer();
}
