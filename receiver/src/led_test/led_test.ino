
int commsLEDPin = D6; 
const int resolution = 8;
const int freq = 5000;
const int LEDChannel = 4;
int last_blink = 0;

void setup() {
  pinMode(commsLEDPin, OUTPUT);
  //ledcAttach(commsLEDPin, freq, resolution);
  //ledcAttach(LED_PIN, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
}

void loop() {
  digitalWrite(commsLEDPin, HIGH); // turn the LED on
  delay(500);             // wait for 500 milliseconds
  digitalWrite(commsLEDPin, LOW);  // turn the LED off
  delay(500);             // wait for 500 milliseconds
  
}
