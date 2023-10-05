#include <Wire.h>
#include <U8g2lib.h>
#include <esp_now.h>
#include <WiFi.h>
#include <nvs_flash.h>
#include <Preferences.h>
#include <SimpleCLI.h>
#include <PreferencesCLI.h>
#include "hardware.h"
#include "common.h"
#include "screen_icons.h"
#include "joystick.h"

#define SOFTWARE_VERSION "v0.1"

#define CONNECTION_TIMEOUT_MS 1000

//globals from libraries
Preferences preferences;
SimpleCLI cli;
PreferencesCLI prefCli(preferences);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C screen(U8G2_R2);

//globals for transmitter states
TransmitterState currentState = TRANSMITTER_STATE_BOOT;
TransmitterFault currentFaults = TRANSMITTER_FAULT_NONE;
TransmitterWarning currentWarnings = TRANSMITTER_WARNING_NONE;

//globals for communication
esp_now_peer_info_t receiverCommsInfo;  
CommandMessage commandMessage;          //outgoing
ResponseMessage responseMessage;        //incoming
long responseMessageTime;
long roundtripTime;         

//globals for hardware
Joystick leftJoystick;
Joystick rightJoystick;
float batteryVoltage;
float batteryVoltageMultiplier;

//globals for CLI
char cliResponseBuffer[256];
Command helpCommand;//command used to display CLI help
Command restartCommand;//command used to restart the transmitter or receiver
Command getVarCommand;//used to return values of certain internal variables
long restartTime;//if non zero, board will restart if millis() > restart

void setup() {
  currentState = TRANSMITTER_STATE_STARTUP;
  bool startupOK = true;

  Serial.begin(115200);
  while (!Serial);//wait for serial to begin
  Serial.println("Vector Space {Combat Robot}");
  Serial.println(SOFTWARE_VERSION);
  Serial.println("https://github.com/VectorSpaceHQ/VS_combat_robot\r\n");

  startupOK &= leftJoystick.setup(PIN_LEFT_JOYSTICK,preferences,"LeftJoystick", true);
  startupOK &= rightJoystick.setup(PIN_RIGHT_JOYSTICK,preferences,"RightJoystick", true);

  pinMode(PIN_WEAPON_TOGGLE_SWITCH,INPUT_PULLUP);
  pinMode(PIN_RIGHT_THUMB_SWITCH,INPUT_PULLUP);
  pinMode(PIN_RIGHT_TRIGGER,INPUT_PULLUP);
  pinMode(PIN_LEFT_TRIGGER,INPUT_PULLUP);
  pinMode(PIN_BATTERY_SENSE,INPUT_PULLUP);
  
  pinMode(PIN_FAULT_LED,OUTPUT);
  digitalWrite(PIN_FAULT_LED,LOW);
  pinMode(PIN_COMMS_LED,OUTPUT);
  digitalWrite(PIN_COMMS_LED,LOW);

  preferences.begin("Battery");
  batteryVoltageMultiplier = preferences.getFloat("Multiplier",DEFAULT_BATTERY_VOLTAGE_MULTIPLIER);
  preferences.end();

  startupOK &= screenSetup();
  startupOK &= espNowSetup();
  startupOK &= cliSetup();

  if(!startupOK)
  {
    currentState = TRANSMITTER_STATE_CRITICAL_FAULT;
    Serial.println("ERROR: Critical fault during startup is preventing transition to normal operation");
    digitalWrite(PIN_FAULT_LED,HIGH);
  } else {
    currentState = TRANSMITTER_STATE_CONNECTING;
    Serial.println("Startup Successful, transitioning to connecting state");
    digitalWrite(PIN_FAULT_LED,LOW);
  }
  
    Serial.println();
    Serial.print("> ");
}

void loop() {

  leftJoystick.loop();
  rightJoystick.loop();

  batteryVoltage = batteryVoltageMultiplier * (analogReadMilliVolts(PIN_BATTERY_SENSE) / 1000.0);

  if(currentState & (TRANSMITTER_STATE_OPERATION | TRANSMITTER_STATE_CONNECTING))
  {
    if(millis() - commandMessage.send_time > 50){
      sendCommand();
    }
    
  }

  if(currentState == TRANSMITTER_STATE_OPERATION)
  {
    if((millis() - responseMessageTime) > CONNECTION_TIMEOUT_MS)
    {
      Serial.println("ERROR: Lost connection with receiver");
      currentState = TRANSMITTER_STATE_CONNECTING;
      digitalWrite(PIN_COMMS_LED,LOW);
    }
  }

  if(currentState == TRANSMITTER_STATE_CONNECTING)
  {
    if((millis() - responseMessageTime) < CONNECTION_TIMEOUT_MS)
    {
      Serial.println("Receiver response received. Transitioning to operation state");
      currentState = TRANSMITTER_STATE_OPERATION;
      digitalWrite(PIN_COMMS_LED,HIGH);
    }
  }

  

  if(restartTime > 0 && millis() > restartTime)
  {
    ESP.restart();
  }

  if(Serial.available())
  {
    String cliInput = Serial.readStringUntil('\n');
    Serial.println(cliInput);
    cli.parse(cliInput);
    if(cli.available())
    {
      Command command = cli.getCommand();
      prefCli.handleCommand(command,Serial);
    }
    Serial.println();
    Serial.print("> ");
  }


  update_screen();
}

bool sendCommand() {
  commandMessage.id++;
  commandMessage.send_time = millis();
  commandMessage.left_speed = leftJoystick.getValue();
  commandMessage.right_speed = rightJoystick.getValue();
  commandMessage.weapon_speed = digitalRead(PIN_WEAPON_TOGGLE_SWITCH) ? DEFAULT_WEAPON_SPEED : 0;
  commandMessage.horn_frequency = digitalRead(PIN_RIGHT_THUMB_SWITCH) ? 0 : DEFAULT_HORN_FREQUENCY;
  commandMessage.servo_position = 0;
  commandMessage.clear_faults = RECEIVER_FAULT_NONE;
  commandMessage.clear_warnings = RECEIVER_WARNING_NONE;

  esp_err_t result = esp_now_send(receiverCommsInfo.peer_addr, (uint8_t *) &commandMessage, sizeof(commandMessage));
  if(result != ESP_OK)
  {
    Serial.println("WARNING: command message may not have been sent");
  }
  return result == ESP_OK;

  commandMessage.command = REMOTE_COMMAND_NONE;//clear out all commands (they are oneshots)
}


bool espNowSetup() {
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.print("Transmitter WiFi Initialized at MAC "); Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ERROR: ESPNOW failed to init");
    return false;
  }
  

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
    // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  preferences.begin("Comms");
  if(preferences.isKey("Address"))
  {
    size_t addressLength = preferences.getBytes("Address",receiverCommsInfo.peer_addr,6);
    preferences.end();
    if(addressLength != 6)
    {
      Serial.println("ERROR: receiver address stored in Comms/Address is not 6 bytes long");
      return false;
    } 
  } else {
    Serial.println("WARNING: No receiver address stored in Comms/Address");
  }
  receiverCommsInfo.channel = 0;
  receiverCommsInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&receiverCommsInfo) != ESP_OK)
  {
    Serial.println("ERROR: Failed to add receiver as peer");
    return false;
  } 
  char messageBuffer[255];
  sprintf(messageBuffer,"Receiver Configured at MAC %02X:%02X:%02X:%02X:%02X:%02X",
    receiverCommsInfo.peer_addr[0],
    receiverCommsInfo.peer_addr[1],
    receiverCommsInfo.peer_addr[2],
    receiverCommsInfo.peer_addr[3],
    receiverCommsInfo.peer_addr[4],
    receiverCommsInfo.peer_addr[5]);
  Serial.println(messageBuffer);

  commandMessage.id = 1;
  commandMessage.send_time = millis();

  return true;
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  roundtripTime = 0;
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&responseMessage, incomingData, sizeof(responseMessage));
  responseMessageTime = millis();
  if(responseMessage.command_id == commandMessage.id)
  {
    roundtripTime = millis() - commandMessage.send_time;
  } else {
    Serial.println("ERROR: response message received out of order");
  }
}



bool cliSetup()
{
  cli.setErrorCallback(cliErrorCallback);

  helpCommand = cli.addCommand("help", helpCommandCallback);
  helpCommand.setDescription("Use to display details on how to use a specific command\r\nAvailable Commands:\r\n\tsetPreference (setp)\r\n\tgetPreference (getp)\r\n\tclearPreference (clearp)\r\n\trestart\r\n\tgetVariable");
  helpCommand.addPositionalArgument("commandName,cmd","");

  prefCli.registerCommands(cli);

  restartCommand = cli.addCommand("restart", restartCommandCallback);
  restartCommand.setDescription("Used to restart the transmitter (-t) and/or the receiver (-r)");
  restartCommand.addFlagArgument("t/ransmitter");
  restartCommand.addFlagArgument("r/eceiver");

  getVarCommand = cli.addCommand("getV/ar/iable,getv", getVariableCommandCallback);
  getVarCommand.setDescription("Used to get the current value of the specified variable");
  getVarCommand.addPositionalArgument("v/ar/iable");

  return true;
}

void cliErrorCallback(cmd_error* e)
{
  CommandError cmdError(e);
  Serial.print("ERROR: ");
  Serial.println(cmdError.toString());

  if (cmdError.hasCommand()) {
      Serial.print(cmdError.getCommand().toString());
  }
}

void helpCommandCallback(cmd* commandPointer)
{
  Command cmd(commandPointer);
  Argument commandName = cmd.getArg("commandName");
  if (commandName.isSet())
  {
    Command namedCommand = cli.getCommand(commandName.getValue());
    if (namedCommand.getName())
    {
      Serial.println(namedCommand.toString());
    } else {
      sprintf(cliResponseBuffer,"No matching command found for name '%s'\r\n%s",namedCommand.toString(),helpCommand.toString());
      Serial.println(cliResponseBuffer);
    }
  } else {
    Serial.println(helpCommand.toString());
  }
}

void restartCommandCallback(cmd* commandPointer)
{
  Command cmd(commandPointer);
  Argument transmitter = cmd.getArg("transmitter");
  Argument receiver = cmd.getArg("receiver");

  if(receiver.isSet())
  {
    commandMessage.command = (RemoteCommand)(commandMessage.command | REMOTE_COMMAND_REBOOT);
    Serial.println("Remote restart queued.");
  }

  if(transmitter.isSet())
  {
    restartTime = millis()+1000;
    Serial.println("Local restart queued.");
  }

}

void getVariableCommandCallback(cmd* commandPointer)
{
  Command cmd(commandPointer);
  Argument variable = cmd.getArg("variable");
  String variableName = variable.getValue();

  if(variableName.equalsIgnoreCase("millis")) Serial.println(millis());
  else if (variableName.equalsIgnoreCase("commandMessageId")) Serial.println(commandMessage.id);
  else if (variableName.equalsIgnoreCase("commandMessageTime")) Serial.println(commandMessage.id);
  else if (variableName.equalsIgnoreCase("responseMessageId")) Serial.println(responseMessage.command_id);
  else if (variableName.equalsIgnoreCase("leftSpeed")) Serial.println(commandMessage.left_speed);
  else if (variableName.equalsIgnoreCase("rightSpeed")) Serial.println(commandMessage.right_speed);
  else if (variableName.equalsIgnoreCase("leftStickV")) Serial.println(leftJoystick.getVoltage());
  else if (variableName.equalsIgnoreCase("rightStickV")) Serial.println(rightJoystick.getVoltage());
  else if (variableName.equalsIgnoreCase("weaponSpeed")) Serial.println(commandMessage.weapon_speed);
  else if (variableName.equalsIgnoreCase("batteryVoltage")) Serial.println(batteryVoltage);
  else if (variableName.equalsIgnoreCase("hornFreq")) Serial.println(commandMessage.horn_frequency);
  else Serial.println("Variable is not supported by this command");
}

bool screenSetup(){
  return screen.begin();
}

void update_screen(){
  screen.clearBuffer();

  //LEFT sidebar for controller data
  screen.drawBox(16,0,2,64);
  screen.drawXBMP(2,2,12,7,screen_icon_controller);

  //RIGHT sidebar for robot data
  screen.drawBox(109,0,2,64);
  screen.drawXBMP(113,2,12,7,screen_icon_robot);

  screen.sendBuffer();
}
