int buzzerPin = D4; // GPIO4

void setup() {
  // sets the pins as outputs:
  pinMode(buzzerPin, OUTPUT);
}

void loop(){
  digitalWrite(buzzerPin, LOW);
  delay(100);
  digitalWrite(buzzerPin, HIGH); 
  delay(200);
}
