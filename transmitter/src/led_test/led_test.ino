#define PIN_FAULT_LED D6
#define PIN_COMMS_LED D7

void setup()
{
        pinMode(PIN_FAULT_LED,OUTPUT);
        pinMode(PIN_COMMS_LED,OUTPUT);
}

void loop()
{
        digitalWrite(PIN_FAULT_LED, LOW);
        digitalWrite(PIN_COMMS_LED, LOW);
        delay(2000);
        digitalWrite(PIN_FAULT_LED, HIGH);
        delay(1000);
        digitalWrite(PIN_COMMS_LED, HIGH);
        delay(1000);
}
