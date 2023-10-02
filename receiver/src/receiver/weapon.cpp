#include "Arduino.h"
#include "weapon.h"


Weapon::Weapon(int Pin){
    _pin = Pin;
    _value = 0;
    _isSetup = true;
    _armed = false;
}


void Weapon::arm() // https://www.helifreak.com/showthread.php?t=412147
{
    _motor.write(_pin, 30);        // zero throttle
    delay(4000);
    _motor.write(_pin, 90);        // mid throttle low tone
    delay(2000);
    _motor.write(_pin, 30);        // set the servo position (degrees)
    delay(200);
    _armed = true;
}

void Weapon::on(){
    _motor.write(_pin, 120);
}

void Weapon::off(){
    _motor.write(_pin, 40);
}
