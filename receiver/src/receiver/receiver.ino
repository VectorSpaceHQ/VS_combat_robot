#include <WiFi.h>
#include <esp_now.h>
#include <Servo.h> // https://wokwi.com/projects/350037178957431378


uint8_t broadcastAddress[] = {0xD4, 0xF9, 0x8D, 0x03, 0x76, 0x20};

int buzzerPin = D4; // GPIO4
int motor1Pin1 = D2; 
int motor1Pin2 = D3; 
int motor2Pin1 = D0; 
int motor2Pin2 = D1;

// Setting PWM properties
const int freq = 5000;
const int M1C1 = 0;
const int M1C2 = 1;
const int M2C1 = 2;
const int M2C2 = 3;
const int buzzerChannel = 4;
const int resolution = 8;
int dutyCycle;

Servo myservo = Servo();

// Variable to store if sending data was successful
String success;

typedef struct struct_message {
  int leftForward;
  int leftBackward;
  int rightForward;
  int rightBackward;
  bool weapon;
} struct_message;
struct_message myData;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  Serial.print(myData.leftForward);
  Serial.print(", ");
  Serial.print(myData.leftBackward);
  Serial.print(", ");
  Serial.print(myData.rightForward);
  Serial.print(", ");
  Serial.print(myData.rightBackward);
  Serial.print(", ");
  Serial.println(myData.weapon);
}


void arm_brushless() // https://www.helifreak.com/showthread.php?t=412147
{
  myservo.write(servoPin, 20);        // zero throttle
  delay(4000);
  myservo.write(servoPin, 90);        // mid throttle low tone
  delay(2000);
  myservo.write(servoPin, 20);        // set the servo position (degrees)
  delay(200);
}


void setup() {
  Serial.begin(115200);
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
  //GPIO.func_out_sel_cfg[motor1Pin2].inv_sel = 1; 
  
  sound_on();
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.macAddress());  

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    sound_error();
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  sound_ready();
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

void sound_error(){/*
  tone(buzzerPin, 550, 250);
  delay(7);
  tone(buzzerPin, 150, 600);
  noTone(buzzerPin);
  delay(10);*/
}


void loop(){

  if (myData.leftForward>0){
    dutyCycle = map(myData.leftForward, 0, 100, 0, 255);
    Serial.print("Forward: ");
    Serial.println(dutyCycle);
    ledcWrite(M1C1, dutyCycle);  
    ledcWrite(M1C2, 0); 
  }
  else if (myData.leftBackward>0){
    dutyCycle = map(myData.leftBackward, 0, 100, 0, 255);
    Serial.print("Backward: ");
    Serial.println(dutyCycle);
    ledcWrite(M1C1, 0);  
    ledcWrite(M1C2, dutyCycle); 
  }
  else{
    ledcWrite(M1C1, 255); 
    ledcWrite(M1C2, 255); 
    Serial.println("STOP");
  }

  if (myData.rightForward>0){
    dutyCycle = map(myData.rightForward, 0, 100, 0, 255);
    Serial.print("right Forward: ");
    Serial.println(dutyCycle);
    ledcWrite(M2C1, dutyCycle);  
    ledcWrite(M2C2, 0); 
  }
  else if (myData.rightBackward>0){
    dutyCycle = map(myData.rightBackward, 0, 100, 0, 255);
    Serial.print("right Backward: ");
    Serial.println(dutyCycle);
    ledcWrite(M2C1, 0);  
    ledcWrite(M2C2, dutyCycle); 
  }
  else{
    ledcWrite(M2C1, 255); 
    ledcWrite(M2C2, 255); 
    Serial.println("STOP");
  }
  

  delay(10);
  
}
