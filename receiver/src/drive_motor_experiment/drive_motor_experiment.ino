#include "driver/ledc.h"
#include "esp_err.h"
//https://esp32.com/viewtopic.php?t=821


int motor1Pin1 = D2; 
int motor1Pin2 = D3; 
int motor2Pin1 = D0; 
int motor2Pin2 = D1; 

const int M1C1 = 0;
const int M1C2 = 2;
const int M2C1 = 1;
const int M2C2 = 4;


void setup() {
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);


  ledc_timer_config_t timer1;
  timer1.speed_mode = LEDC_LOW_SPEED_MODE;
  timer1.duty_resolution = LEDC_TIMER_8_BIT;
  timer1.timer_num = LEDC_TIMER_0;
  timer1.freq_hz = 5000;
  ledc_timer_config(&timer1);

ledc_timer_config_t timer2;
  timer2.speed_mode = LEDC_LOW_SPEED_MODE;
  timer2.duty_resolution = LEDC_TIMER_8_BIT;
  timer2.timer_num = LEDC_TIMER_1;
  timer2.freq_hz = 5000;
  ledc_timer_config(&timer2);

  ledc_channel_config_t chan1;
  chan1.gpio_num = 2; // D0
  chan1.speed_mode = LEDC_LOW_SPEED_MODE;
  chan1.channel = LEDC_CHANNEL_0;
  chan1.timer_sel = LEDC_TIMER_0;
  chan1.duty = 0;
  ledc_channel_config(&chan1);

  ledc_channel_config_t chan2;
  chan2.gpio_num = 3; // D1
  chan2.speed_mode = LEDC_LOW_SPEED_MODE;
  chan2.channel = LEDC_CHANNEL_1;
  chan2.timer_sel = LEDC_TIMER_1;
  chan2.duty = 150;
  ledc_channel_config(&chan2);
  
}

void loop() { 

  
}
