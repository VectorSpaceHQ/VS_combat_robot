int buzzerPin = 6; // GPIO4, D4
const int resolution = 8;
const int freq = 5000;
const int buzzerChannel = 4;

void setup() {
  // sets the pins as outputs:
  pinMode(buzzerPin, OUTPUT);

  ledcSetup(buzzerChannel, freq, resolution);
  ledcAttachPin(buzzerPin, buzzerChannel);

   sound_on();
  delay(2000);

  sound_ready();
  delay(2000);

  sound_error();
  delay(2000);
}


void sound_on(){
  // Do not use tone as it interferes with ledc and the motor control
  ledcWriteTone(buzzerChannel, 247);
  delay(250);
  ledcWriteTone(buzzerChannel, 494);
  delay(500);
  ledcWriteTone(buzzerChannel, 0);
  delay(20);
}

void sound_ready(){
  ledcWriteTone(buzzerChannel, 400);
  delay(250);
  ledcWriteTone(buzzerChannel, 400);
  delay(250);
  ledcWriteTone(buzzerChannel, 2000);
  delay(500);
  ledcWriteTone(buzzerChannel, 0);
  delay(20);
  }

void sound_error(){
  ledcWriteTone(buzzerChannel, 550);
  delay(250);
  ledcWriteTone(buzzerChannel, 150);
  delay(600);
  ledcWriteTone(buzzerChannel, 0);
  delay(20);
}

void loop(){


 
  
}
