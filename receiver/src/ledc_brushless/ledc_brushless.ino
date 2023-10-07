#include "driver/ledc.h"
#include "esp_err.h"
// https://github.com/espressif/esp-idf/blob/272b4091f1f1ff169c84a4ee6b67ded4a005a8a7/examples/peripherals/ledc/ledc_basic/main/ledc_basic_example_main.c

ledc_channel_t pwm_channel = LEDC_CHANNEL_5;
ledc_timer_t timer_channel = LEDC_TIMER_2;

int max_value = pow(2,13) - 1;
// no throttle is 5.6%, full throttle is 9.4% duty cycle
  int no_throttle = max_value * 0.056;
  int full_throttle = max_value * 0.094;
  int neutral_throttle = max_value * 0.075;

void setup(){

    Serial.begin(115200);
     int PinA = D5;
     pinMode(PinA, OUTPUT);
     delay(3000);

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_13_BIT,
        .timer_num        = timer_channel,
        .freq_hz          = 50,  // Set output frequency at 50 Hz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel1 = {
        .gpio_num       = PinA,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = pwm_channel,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = timer_channel,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel1));


    arm();


    
}

void loop(){
  Serial.println("full");
  ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_channel, full_throttle) );
     ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_channel) );
  delay(2000);

  Serial.println("neutral");
  ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_channel,neutral_throttle) );
     ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_channel) );
  delay(2000);

  Serial.println("off");
  ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_channel, no_throttle) );
     ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_channel) );
  delay(2000);
}


void arm(){
  

  Serial.println("arming");
     ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_channel, neutral_throttle) );
     ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_channel) );
     Serial.println("step 1 done");
    delay(1000);
    


    ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_channel, no_throttle) );
     ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_channel) );
     Serial.println("step 3 done");
    delay(1000);
    
}
