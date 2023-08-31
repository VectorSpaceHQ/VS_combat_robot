int buzzerPin = D4; // GPIO4

void setup() {
  // sets the pins as outputs:
  pinMode(buzzerPin, OUTPUT);
}


void sound_on(){
  tone(buzzerPin, 247, 250);
  delay(4 * 1.3);
  tone(buzzerPin, 494, 250);
  noTone(buzzerPin);
}

void sound_ready(){
  tone(buzzerPin, 400, 250);
  delay(7);
  tone(buzzerPin, 400, 250);
  delay(7);
  tone(buzzerPin, 2000, 500);
  noTone(buzzerPin);
  }

void sound_error(){
  tone(buzzerPin, 550, 250);
  delay(7);
  tone(buzzerPin, 150, 600);
  noTone(buzzerPin);
}

void loop(){


  sound_on();
  delay(2000);

  sound_ready();
  delay(2000);

  sound_error();
  delay(2000);
  
}
