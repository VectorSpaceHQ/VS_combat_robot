// define led according to pin diagram
int led = D4; // GPIO4

void setup() {
  // initialize digital pin led as an output
  pinMode(led, OUTPUT);
}

void loop() {
  digitalWrite(led, HIGH);   // turn the LED on 
  delay(500);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off
  delay(500);               // wait for a second
}
