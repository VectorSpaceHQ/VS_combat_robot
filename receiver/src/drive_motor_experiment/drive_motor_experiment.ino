#include "driver/ledc.h"
#include "esp_err.h"
#include "brush_motor.h"
#include "weapon.h"

//https://esp32.com/viewtopic.php?t=821
// https://www.esp32.com/viewtopic.php?f=2&t=821
// https://github.com/espressif/esp-idf/blob/272b4091f1f1ff169c84a4ee6b67ded4a005a8a7/examples/peripherals/ledc/ledc_basic/main/ledc_basic_example_main.c


Motor Lmotor;
Motor Rmotor;
Weapon weapon(D5);

void setup() {

  Serial.begin(115200);
  //delay(500);
  weapon.arm();

  // weapon and Lmotor are in conflict (chan2, chan3). ESC won't arm
  // same with Rmotor

  Lmotor.setup(D0, D1, LEDC_TIMER_0, LEDC_CHANNEL_2, LEDC_CHANNEL_3);
  Rmotor.setup(D2, D3, LEDC_TIMER_0, LEDC_CHANNEL_0, LEDC_CHANNEL_1);

  weapon.on();

}

void loop() {
     for (int i=0; i<250; i+=10){
         Lmotor.set_speed(1, i);
         Rmotor.set_speed(1, i);
         delay(100);
     }
}
