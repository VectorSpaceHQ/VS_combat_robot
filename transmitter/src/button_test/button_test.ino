#define PIN_LEFT_JOYSTICK A1
#define PIN_RIGHT_JOYSTICK A2
#define PIN_LEFT_TRIGGER D9
#define PIN_RIGHT_TRIGGER D8
#define PIN_RIGHT_THUMB_SWITCH D3
#define PIN_WEAPON_TOGGLE_SWITCH D10

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting up...");
    pinMode(PIN_WEAPON_TOGGLE_SWITCH,INPUT_PULLUP);
    pinMode(PIN_RIGHT_THUMB_SWITCH,INPUT_PULLUP);
    pinMode(PIN_RIGHT_TRIGGER,INPUT_PULLUP);
    pinMode(PIN_LEFT_TRIGGER,INPUT_PULLUP);
}

void loop()
{
     if (digitalRead(PIN_WEAPON_TOGGLE_SWITCH) == 1){
        Serial.println("Weapon ON");
     }
     if (digitalRead(PIN_RIGHT_THUMB_SWITCH) == 0){
        Serial.println("Right Thumb switch ON");
     }
     if (digitalRead(PIN_RIGHT_TRIGGER) == 0){
        Serial.println("Right trigger ON");
     }
     if (digitalRead(PIN_LEFT_TRIGGER) == 0){
        Serial.println("Left Trigger ON");
     }
}
