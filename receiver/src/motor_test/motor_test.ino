/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
  https://randomnerdtutorials.com/esp32-dc-motor-l298n-motor-driver-control-speed-direction/
*********/

// Motor A

int motor1Pin1 = D2; 
int motor1Pin2 = D3; 
int motor2Pin1 = D0; 
int motor2Pin2 = D1; 

int buzzerPin = D4; // GPIO4

// Setting PWM properties
const int freq = 5000;
const int M1C1 = 0;
const int M1C2 = 1;
const int M2C1 = 2;
const int M2C2 = 3;
const int resolution = 8;
int dutyCycle = 0;
const int buzzerChannel = 4;


void sound_on(){
  // Do not use tone as it interferes with ledc and the motor control
  ledcWriteTone(buzzerChannel, 247);
  delay(250);
  ledcWriteTone(buzzerChannel, 494);
  delay(500);
  ledcWriteTone(buzzerChannel, 0);
  delay(20);
}


void setup() {
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  
  // configure LED PWM functionalitites
  ledcSetup(M1C1, freq, resolution);
  ledcSetup(M1C2, freq, resolution);
  ledcSetup(M2C1, freq, resolution);
  ledcSetup(M2C2, freq, resolution);
  ledcAttachPin(motor1Pin1, M1C1);
  ledcAttachPin(motor1Pin2, M1C2);
  ledcAttachPin(motor2Pin1, M2C1);
  ledcAttachPin(motor2Pin2, M2C2);
  ledcSetup(buzzerChannel, freq, resolution);
  ledcAttachPin(buzzerPin, buzzerChannel);

  sound_on();

  Serial.begin(115200);

  // testing
  Serial.print("Testing DC Motor...");
}

void loop() { 
  while (dutyCycle <= 255){
    ledcWrite(M1C1, dutyCycle);  
    ledcWrite(M1C2,0); 
    ledcWrite(M2C1, dutyCycle);  
    ledcWrite(M2C2,0); 
    Serial.print("Forward with duty cycle: ");
    Serial.println(dutyCycle);
    dutyCycle = dutyCycle + 10;
    delay(500);
  }
  dutyCycle = 0;

  while (dutyCycle <= 255){
    ledcWrite(M1C1, 0);  
    ledcWrite(M1C2, dutyCycle); 
    ledcWrite(M2C1, 0);  
    ledcWrite(M2C2, dutyCycle); 
    Serial.print("Backward with duty cycle: ");
    Serial.println(dutyCycle);
    dutyCycle = dutyCycle + 10;
    delay(500);
  }
  dutyCycle = 0;

  Serial.println("Stop");
  ledcWrite(M1C1, 255); 
  ledcWrite(M1C2, 255); 
  ledcWrite(M2C1, 255); 
  ledcWrite(M2C2, 255); 
  delay(3000);
  
}
