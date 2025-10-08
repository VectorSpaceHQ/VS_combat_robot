#ifndef HARDWARE_H
#define HARDWARE_H
#include "Arduino.h"

// Pin numbers are GPIO numbering scheme. Translation comes from official pinout diagram.

#define PIN_LEFT_MOTOR_FORWARD 4 //D2
#define PIN_LEFT_MOTOR_BACKWARD 5 // D3
#define PIN_RIGHT_MOTOR_FORWARD 2 // D0
#define PIN_RIGHT_MOTOR_BACKWARD 3 // D1

#define PIN_PAIR_BUTTON D9

#define PWM_PIN_LEFT  10 // D10
#define PWM_PIN_RIGHT 20 // D7

#define PWM_CHANNEL_LEFT 1
#define PWM_CHANNEL_RIGHT 2

#define PIN_COMMS_LED D8
#define PIN_OPT_LED D7
#define SEL0 21 // D6

#endif
