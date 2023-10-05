#include "driver/ledc.h"
#include "esp_err.h"
// https://github.com/espressif/esp-idf/blob/272b4091f1f1ff169c84a4ee6b67ded4a005a8a7/examples/peripherals/ledc/ledc_basic/main/ledc_basic_example_main.c


void setup(){

    Serial.begin(115200);
     int PinA = D5;       
     pinMode(PinA, OUTPUT); 
     delay(3000); 

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_13_BIT,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = 50,  // Set output frequency at 50 Hz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel1 = {
        .gpio_num       = PinA,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER_0,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel1));


    arm();


    ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 4000) );
     ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0) );
}

void loop(){
  Serial.println("alive");
  delay(2000);
}


void arm(){
  Serial.println("arming");
     ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 500) );
     ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0) );
    delay(4000);
    Serial.println("step 1 done");
    
    ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 4000) );
     ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0) );
    delay(2000);
    Serial.println("step 2 done");
    
    ESP_ERROR_CHECK( ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 500) );
     ESP_ERROR_CHECK( ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0) );
    delay(200);
    Serial.println("step 3 done");
}
