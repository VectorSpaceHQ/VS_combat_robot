#include "led_matrix.h"
#include "CountUpDownTimer.h" // AndrewMascolo

// #include <MatrixHardware_ESP32_V0.h>                // This file contains multiple ESP32 hardware configurations, edit the file to define GPIOPINOUT (or add #define GPIOPINOUT with a hardcoded number before this #include)
// #define USE_ADAFRUIT_GFX_LAYERS
// #include <SmartMatrix.h> // by pixelmatix, https://github.com/pixelmatix/SmartMatrix

// https://esp32io.com/tutorials/esp32-led-matrix


CountUpDownTimer matchTimer(DOWN);

LEDMatrix::LEDMatrix(int SDA, int SCL){
    _sda_pin = SDA;
    _scl_pin = SCL;
    this->reset();
    matchTimer.StartTimer();

}

void LEDMatrix::begin_countdown(){
    if(_ready){
        Serial.println("countdown starting now");

        // if(matchTimer.ShowSeconds() == 180){
        //     matchTimer.StartTimer();
        // }
    }
    matchTimer.Timer();

    _ready = false;
}

void LEDMatrix::reset(){
    int minute = 03; //Sets the minute variable
    int second = 00; //Sets the second variable
    matchTimer.SetTimer(0, 0, minute, second);

    _ready = true;
}

void LEDMatrix::get_time(){
    // matchTimer.Timer();
    Serial.print("time remaining: ");
    Serial.print(matchTimer.ShowMinutes());
    Serial.print(":");
    Serial.println(matchTimer.ShowSeconds());
}
