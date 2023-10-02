#include <Wire.h>
#include <U8g2lib.h>
#include <esp_now.h>
#include <WiFi.h>
#include <nvs_flash.h>
#include <Preferences.h>
#include <SimpleCLI.h>
#include "hardware.h"
#include "common.h"
#include "screen_icons.h"
#include "joystick.h"

#define SOFTWARE_VERSION "v0.1"

//globals from libraries
Preferences preferences;
SimpleCLI cli;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C screen(U8G2_R2);

//globals for transmitter states
TransmitterState currentState = TRANSMITTER_STATE_BOOT;
TransmitterFault currentFaults = TRANSMITTER_FAULT_NONE;
TransmitterWarning currentWarnings = TRANSMITTER_WARNING_NONE;

//globals for communication
esp_now_peer_info_t receiverCommsInfo;  
CommandMessage commandMessage;          //outgoing
ResponseMessage responseMessage;        //incoming

//globals for hardware
Joystick leftJoystick;
Joystick rightJoystick;

//globals for CLI
char cliResponseBuffer[256];
Command helpCommand;//command used to display CLI help
Command setPrefCommand;//command used to set preferences
Command getPrefCommand;//command used to set preferences
Command clearPrefCommand;//command used to clear preferences, namespaces, and the whole storage area
Command restartCommand;//command used to restart the transmitter or receiver
Command getVarCommand;//used to return values of certain internal variables
long restartTime;//if non zero, board will restart if millis() > restart

void setup() {
  currentState = TRANSMITTER_STATE_STARTUP;
  bool startupOK = true;

  pinMode(PIN_WEAPON_TOGGLE_SWITCH,INPUT_PULLUP);
  pinMode(PIN_RIGHT_THUMB_SWITCH,INPUT_PULLUP);
  pinMode(PIN_RIGHT_TRIGGER,INPUT_PULLUP);
  pinMode(PIN_LEFT_TRIGGER,INPUT_PULLUP);
  pinMode(PIN_BATTERY_SENSE,INPUT);
  
  pinMode(PIN_FAULT_LED,OUTPUT);
  pinMode(PIN_COMMS_LED,OUTPUT);

  digitalWrite(PIN_FAULT_LED, LOW);
  digitalWrite(PIN_COMMS_LED, LOW);
  delay(4000);
  digitalWrite(PIN_FAULT_LED, HIGH);
  digitalWrite(PIN_COMMS_LED, HIGH);
  delay(4000);

  Serial.begin(115200);
  //while (!Serial);//wait for serial to begin
  Serial.println("Vector Space {Combat Robot}");
  Serial.println(SOFTWARE_VERSION);
  Serial.println("https://github.com/VectorSpaceHQ/VS_combat_robot\r\n");

  startupOK &= leftJoystick.setup(PIN_LEFT_JOYSTICK,preferences,"LeftJoystick");
  startupOK &= rightJoystick.setup(PIN_RIGHT_JOYSTICK,preferences,"RightJoystick");

  startupOK &= screenSetup();
  startupOK &= espNowSetup();
  startupOK &= cliSetup();

  if(!startupOK)
  {
    currentState = TRANSMITTER_STATE_CRITICAL_FAULT;
    Serial.println("ERROR: Critical fault during startup is preventing transition to normal operation");
  } else {
    currentState = TRANSMITTER_STATE_OPERATION;
    Serial.println("Startup Successful, transitioning to normal operation");
  }
  
    Serial.println();
    Serial.print("> ");
}

void loop() {

  leftJoystick.loop();
  rightJoystick.loop();
  update_screen();

  if(currentState == TRANSMITTER_STATE_OPERATION)
  {
    if(millis() - commandMessage.send_time > 50){
      sendCommand();
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
    Serial.println();
    Serial.print("> ");
  }

}

bool sendCommand() {
  commandMessage.id++;
  commandMessage.send_time = millis();
  commandMessage.left_speed = leftJoystick.getValue();
  commandMessage.right_speed = rightJoystick.getValue();
  commandMessage.weapon_speed = digitalRead(PIN_WEAPON_TOGGLE_SWITCH) ? DEFAULT_WEAPON_SPEED : 0;
  commandMessage.horn_frequency = digitalRead(PIN_RIGHT_THUMB_SWITCH) ? DEFAULT_HORN_FREQUENCY : 0;
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
  
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&responseMessage, incomingData, sizeof(responseMessage));

}



bool cliSetup()
{
  cli.setErrorCallback(cliErrorCallback);

  helpCommand = cli.addCommand("help", helpCommandCallback);
  helpCommand.setDescription("Use to display details on how to use a specific command\r\nAvailable Commands:\r\n\tsetPreference (setp)\r\n\tgetPreference (getp)\r\n\tclearPreference (clearp)\r\n\trestart");
  helpCommand.addPositionalArgument("commandName,cmd","");

  setPrefCommand = cli.addCommand("setP/ref/erence,setp", setPreferenceCommandCallback);
  setPrefCommand.setDescription("Used to write the value of entries using the built-in 'Preferences' module.\r\n\tnamespace (-ns): the namespace of the preference setting\r\n\tkey (-k): the string identifier of the preference\r\n\ttype (-t): the type of the data to write\r\n\tvalue (-v): the value to store");
  setPrefCommand.addPositionalArgument("namespace,ns");
  setPrefCommand.addPositionalArgument("k/ey");
  setPrefCommand.addPositionalArgument("t/ype");
  setPrefCommand.addPositionalArgument("v/al/ue");

  getPrefCommand = cli.addCommand("getP/ref/erence,getp", getPreferenceCommandCallback);
  getPrefCommand.setDescription("Used to read the value of entries using the built-in 'Preferences' module.\r\n\tnamespace (-ns): the namespace of the preference setting\r\n\tkey (-k): the string identifier of the preference\r\n\ttype (-t): the type of the data to read");
  getPrefCommand.addPositionalArgument("namespace,ns");
  getPrefCommand.addPositionalArgument("k/ey");
  getPrefCommand.addPositionalArgument("t/ype");

  clearPrefCommand = cli.addCommand("clearP/ref/erence,clearp", clearPreferenceCommandCallback);
  clearPrefCommand.setDescription("Used to clear individual preferences, entire namespaces, or the entire nvs storage area.\r\n\tnamespace (-ns): the namespace of the preference setting\r\n\tkey (-k): the string identifier of the preference");
  clearPrefCommand.addPositionalArgument("namespace,ns","");
  clearPrefCommand.addPositionalArgument("k/ey","");

  restartCommand = cli.addCommand("restart", restartCommandCallback);
  restartCommand.setDescription("Used to restart the transmitter (-t) and/or the receiver (-r)");
  restartCommand.addFlagArgument("t/ransmitter");
  restartCommand.addFlagArgument("r/eceiver");

  getVarCommand = cli.addCommand("getV/ar,getv", getVariableCommandCallback);
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

void setPreferenceCommandCallback(cmd* commandPointer)
{
  Command cmd(commandPointer);
  Argument name = cmd.getArg("namespace");
  Argument key = cmd.getArg("key");
  Argument type = cmd.getArg("type");
  Argument value = cmd.getArg("value");

  preferences.begin(name.getValue().c_str());

  String typeString = type.getValue();
  size_t bytesWritten = 0;
  
  if(typeString.equalsIgnoreCase("Float") )
  {
    bytesWritten = preferences.putFloat(key.getValue().c_str(), value.getValue().toFloat()); 
  } else if(typeString.equalsIgnoreCase("Bytes") ) {
    String valueText = value.getValue();
    valueText.toUpperCase();
    const char * valueTextPtr = valueText.c_str();
    size_t dataLength = (valueText.length()+1)/2;
    char * data = (char*)malloc(dataLength);
    for(int i=0; i < dataLength; ++i)
    {
      char mostSig = valueTextPtr[i*2];
      char leastSig = valueTextPtr[i*2+1];
      data[i] = 16*((mostSig > 64)?(mostSig-55):(mostSig-48)) + ((leastSig > 64)?(leastSig-55):(leastSig-48));
    }
    bytesWritten = preferences.putBytes(key.getValue().c_str(), data, dataLength);
    free(data);
  } else {
    sprintf(cliResponseBuffer,"ERROR: unable to store preferences of type '%s'",typeString.c_str());
    Serial.println(cliResponseBuffer);
  }

  if (bytesWritten > 0){
    sprintf(cliResponseBuffer,"'%s' stored as %s (%d Bytes) in %s/%s",value.getValue(),typeString,bytesWritten,name.getValue().c_str(), key.getValue().c_str());
  } else {
    sprintf(cliResponseBuffer,"ERROR: unable to store '%s' as %s in %s/%s",value.getValue().c_str(),typeString.c_str(),name.getValue().c_str(), key.getValue().c_str());
  }
  Serial.println(cliResponseBuffer);
  preferences.end(); 
}

void getPreferenceCommandCallback(cmd* commandPointer)
{
  Command cmd(commandPointer);
  Argument name = cmd.getArg("namespace");
  Argument key = cmd.getArg("key");
  Argument type = cmd.getArg("type");

  preferences.begin(name.getValue().c_str(),true);

  if(!preferences.isKey(key.getValue().c_str()))
  {
    sprintf(cliResponseBuffer,"ERROR: unable to locate key '%s' in namespace '%s'",key.getValue().c_str(),name.getValue().c_str());
    Serial.println(cliResponseBuffer);
  } else {

    String typeString = type.getValue();
    size_t bytesWritten = 0;
    
    if(typeString.equalsIgnoreCase("Float") )
    {
      Serial.println(preferences.getFloat(key.getValue().c_str()),4); 
    } else if(typeString.equalsIgnoreCase("Bytes")){
      size_t dataLength = preferences.getBytesLength(key.getValue().c_str());
      char * data = (char*)malloc(dataLength);
      char * text = (char*)malloc(dataLength*2+1);
      preferences.getBytes(key.getValue().c_str(), data, dataLength);
      for(int i=0; i < dataLength; ++i)
      {
        char mostSig = data[i] / 16;
        char leastSig = data[i] - 16*mostSig;
        text[i*2] = (mostSig>9)?(55+mostSig):(48+mostSig);
        text[i*2+1] = (leastSig>9)?(55+leastSig):(48+leastSig);
      }
      text[dataLength*2-1] = '\0';
      Serial.println(text);
      free(text);
      free(data);
    } else {
      sprintf(cliResponseBuffer,"ERROR: unable to store preferences of type '%s'",typeString);
      Serial.println(cliResponseBuffer); 
      return;
    }
  }

  preferences.end(); 
}

void clearPreferenceCommandCallback(cmd* commandPointer)
{
  Command cmd(commandPointer);
  Argument name = cmd.getArg("namespace");
  Argument key = cmd.getArg("key");

  if(key.isSet() && name.isSet())
  {
    sprintf(cliResponseBuffer,"'%s' has been cleared from namespace '%s'",key.getValue(),name.getValue());
    if(!preferences.begin(name.getValue().c_str()))
    {
      sprintf(cliResponseBuffer,"ERROR: unable to open namespace '%s'",name.getValue());
    } else 
    {
      if(preferences.isKey(key.getValue().c_str()))
      {
        if(!preferences.remove(key.getValue().c_str()))
        {
          sprintf(cliResponseBuffer,"ERROR: unable to clear '%s' from namespace '%s'",key.getValue(),name.getValue());
        }
      }
      preferences.end();
    }
  } else if (name.isSet())
  {
    sprintf(cliResponseBuffer,"namespace '%s' has been cleared",name.getValue());
    if(!preferences.begin(name.getValue().c_str()))
    {
      sprintf(cliResponseBuffer,"ERROR: unable to open namespace '%s'",name.getValue());
    } else {
      if(!preferences.clear())
      {
        sprintf(cliResponseBuffer,"ERROR: unable to clear namespace '%s'",name.getValue());
      }
      preferences.end();
    }
  } else {
    nvs_flash_erase();
    nvs_flash_init();
    sprintf(cliResponseBuffer,"All preferences have been cleared from the NVS flash");
  }
  Serial.println(cliResponseBuffer);
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
