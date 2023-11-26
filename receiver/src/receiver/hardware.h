#ifndef HARDWARE_H
#define HARDWARE_H
#include "Arduino.h"

#define PIN_LEFT_MOTOR_FORWARD 5 //D3
#define PIN_LEFT_MOTOR_BACKWARD 4 // D2
#define PIN_RIGHT_MOTOR_FORWARD 2 // D0
#define PIN_RIGHT_MOTOR_BACKWARD 3 // D1
#define PIN_BUZZER 6 // 6 = D4 = gpio4

/*
#define PWM_CHANNEL_LEFT_FORWARD 0
#define PWM_CHANNEL_LEFT_BACKWARD 1
#define PWM_CHANNEL_RIGHT_FORWARD 2
#define PWM_CHANNEL_RIGHT_BACKWARD 3
*/
#define PWM_CHANNEL_LEFT_FORWARD 1
#define PWM_CHANNEL_LEFT_BACKWARD 2
#define PWM_CHANNEL_RIGHT_FORWARD 3
#define PWM_CHANNEL_RIGHT_BACKWARD 4
#define PWM_CHANNEL_BUZZER 5

#define PIN_PWR_LED D6 // D4
#define PIN_COMMS_LED D6 


#endif
