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
#include "joystick.h"
#include "auto_pairing.h"
//#include "diagnostics.h"
#include "UiUiUi.h" //DOWNLOAD FROM HERE BECAUSE IT HAS FIXES NOT IN ARDUINO VERSION: https://github.com/burksbuilds/UiUiUi
#include "UIStatusIndicator.h"
#include "UISidebar.h"
#include "UIDashboard.h"
#include "UIMessageBanner.h"
#include "wifi_comms.h"

#define SOFTWARE_VERSION "v1"

#define CONNECTION_TIMEOUT_MS 1000

//globals from libraries
Preferences preferences;
SimpleCLI cli;
PreferencesCLI prefCli(preferences);

//globals for transmitter states
TransmitterState currentState = TRANSMITTER_STATE_BOOT;
TransmitterFault currentFaults = TRANSMITTER_FAULT_NONE;
TransmitterWarning currentWarnings = TRANSMITTER_WARNING_NONE;

//globals for communication
//esp_now_peer_info_t receiverCommsInfo;
CommandMessage cmd_msg;          //outgoing
ResponseMessage rsp_msg;        //incoming


//globals for hardware
Joystick leftJoystick;
Joystick rightJoystick;

//LED fault_led(PIN_FAULT_LED);
//LED comms_led(PIN_COMMS_LED);

uint16_t batteryStateOfCharge;
float batteryVoltage;
float batteryVoltageMultiplier;
float batteryVoltageLowRange;
float batteryVoltageHighRange;
float batteryVoltageAlarmLimit;


//globals for CLI
char cliResponseBuffer[256];
Command helpCommand;//command used to display CLI help
Command restartCommand;//command used to restart the transmitter or receiver
Command getVarCommand;//used to return values of certain internal variables
long restartTime;//if non zero, board will restart if millis() > restart

//globals for screen
U8G2_SSD1306_128X64_NONAME_F_HW_I2C screen(U8G2_R2);
UISidebar robotSidebar = UISidebar(screen_icon_robot);
UIVerticalLine robotDivider = UIVerticalLine(&robotSidebar);

UIMessageBanner messageBanner = UIMessageBanner();
UIHorizontalLine messageBannerDivider = UIHorizontalLine(&messageBanner);
UIDashboard dashboard = UIDashboard(&messageBannerDivider);
UIRows centerArea = UIRows(&dashboard, &robotDivider);

UIVerticalLine controllerDivider = UIVerticalLine(&centerArea);
UISidebar controllerSidebar = UISidebar(screen_icon_controller, &controllerDivider);
UIColumns mainDivider = UIColumns(&controllerSidebar);
UIDisplay displayManager=UIDisplay(&mainDivider);




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
        cmd_msg.command = (RemoteCommand)(cmd_msg.command | REMOTE_COMMAND_REBOOT);
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
  else if (variableName.equalsIgnoreCase("commandMessageId")) Serial.println(cmd_msg.id);
  else if (variableName.equalsIgnoreCase("commandMessageTime")) Serial.println(cmd_msg.id);
  else if (variableName.equalsIgnoreCase("responseMessageId")) Serial.println(rsp_msg.command_id);
  else if (variableName.equalsIgnoreCase("leftSpeed")) Serial.println(cmd_msg.left_speed);
  else if (variableName.equalsIgnoreCase("rightSpeed")) Serial.println(cmd_msg.right_speed);
  else if (variableName.equalsIgnoreCase("leftStickV")) Serial.println(leftJoystick.getVoltage());
  else if (variableName.equalsIgnoreCase("rightStickV")) Serial.println(rightJoystick.getVoltage());
  else if (variableName.equalsIgnoreCase("weaponSpeed")) Serial.println(cmd_msg.weapon_speed);
  else if (variableName.equalsIgnoreCase("batteryVoltage")) Serial.println(batteryVoltage);
  else if (variableName.equalsIgnoreCase("hornFreq")) Serial.println(cmd_msg.horn_frequency);
  else Serial.println("Variable is not supported by this command");
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


bool screenSetup(){
  screen.begin();
  displayManager.init(&screen);
  messageBanner.setMessage(UIStatusIcon::Info,"State Change","Entering Boot State");
  controllerSidebar.updateWifiStrength(-55);
  controllerSidebar.updateStatusIndicator(UIStatusIcon::Warning);
  controllerSidebar.updateBatteryLevel(0x7FFF);
  robotSidebar.updateWifiStrength(-65);
  robotSidebar.updateStatusIndicator(UIStatusIcon::Fault);
  robotSidebar.updateBatteryLevel(0xAFFF);
  return true;
}


void setup() {
  // TESTING -----
  //comms_led.toggle();
  //delay(1000);
  //comms_led.toggle();
  //delay(1000);
  // -------- -----

  currentState = TRANSMITTER_STATE_STARTUP;
  bool startupOK = true;

  Serial.begin(115200);
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
  batteryVoltage = batteryVoltageMultiplier * (analogReadMilliVolts(PIN_BATTERY_SENSE) / 1000.0);

  pinMode(PIN_FAULT_LED,OUTPUT);
  digitalWrite(PIN_FAULT_LED,LOW);
  pinMode(PIN_COMMS_LED,OUTPUT);
  digitalWrite(PIN_COMMS_LED,LOW);

  preferences.begin("Battery");
  batteryVoltageMultiplier = preferences.getFloat("Multiplier",DEFAULT_BATTERY_VOLTAGE_MULTIPLIER);
  batteryVoltageLowRange = preferences.getFloat("LowRange",DEFAULT_BATTERY_LOW_RANGE_VOLTAGE);
  batteryVoltageHighRange = preferences.getFloat("HighRange",DEFAULT_BATTERY_HIGH_RANGE_VOLTAGE);
  batteryVoltageAlarmLimit = preferences.getFloat("AlarmLimit",DEFAULT_BATTERY_ALARM_LIMIT_VOLTAGE);
  preferences.end();

  startupOK &= screenSetup();
  startupOK &= espNowSetup();
  startupOK &= cliSetup();

  if(!startupOK)
  {
    currentState = TRANSMITTER_STATE_CRITICAL_FAULT;
    Serial.println("ERROR: Critical fault during startup is preventing transition to normal operation");
    digitalWrite(PIN_FAULT_LED,HIGH);
    controllerSidebar.updateStatusIndicator(UIStatusIcon::Fault);
    messageBanner.setMessage(UIStatusIcon::Fault,"Critical Fault","Unable to complete startup");
  } else {
    currentState = TRANSMITTER_STATE_CONNECTING;
    Serial.println("Startup Successful, transitioning to connecting state");
    controllerSidebar.updateStatusIndicator(UIStatusIcon::None);
    messageBanner.setMessage(UIStatusIcon::Info,"Startup Complete","Connecting to robot...");
    digitalWrite(PIN_FAULT_LED,LOW);
  }


    Serial.println();
    Serial.print("> ");
}

void loop() {
  cmd_msg = GetCommandMessage();
  rsp_msg = GetResponseMessage();

  leftJoystick.loop();
  rightJoystick.loop();

  batteryVoltage = 0.99 * batteryVoltage + 0.01 * batteryVoltageMultiplier * (analogReadMilliVolts(PIN_BATTERY_SENSE) / 1000.0);
  batteryStateOfCharge = map(static_cast<uint16_t>(batteryVoltage*1000), batteryVoltageLowRange*1000, batteryVoltageHighRange*1000, 0, 0xFFFF);
  controllerSidebar.updateBatteryLevel(batteryStateOfCharge);

  if(currentState & (TRANSMITTER_STATE_OPERATION | TRANSMITTER_STATE_CONNECTING))
  {
    if(millis() - cmd_msg.send_time > 50){
        sendCommand(leftJoystick, rightJoystick);
    }

  }

  if(currentState == TRANSMITTER_STATE_OPERATION)
  {
    robotSidebar.updateStatusIndicator(UIStatusIcon::None);
    robotSidebar.updateWifiStrength(rsp_msg.wifi_strength);
    robotSidebar.updateBatteryLevel(256 * (uint16_t)rsp_msg.battery_charge);
    controllerSidebar.updateWifiStrength(0);//#TODO!
    //comms_led.on();
    if((millis() - rsp_msg.responseMessageTime) > CONNECTION_TIMEOUT_MS)
    {
      Serial.println("ERROR: Lost connection with receiver");
      currentState = TRANSMITTER_STATE_CONNECTING;
      robotSidebar.updateStatusIndicator(UIStatusIcon::Question);
      robotSidebar.updateWifiStrength(0);
      robotSidebar.updateBatteryLevel(0);
      messageBanner.setMessage(UIStatusIcon::Warning,"Robot Lost","Connection Lost");
      controllerSidebar.updateWifiStrength(0);
    }
  }

  if(currentState == TRANSMITTER_STATE_CONNECTING)
  {

    //comms_led.blink();
    if((millis() - rsp_msg.responseMessageTime) < CONNECTION_TIMEOUT_MS)
    {
      Serial.println("Receiver response received. Transitioning to operation state");
      currentState = TRANSMITTER_STATE_OPERATION;
      messageBanner.setMessage(UIStatusIcon::Info,"Robot Connected","Transmitter is Operating");
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

  dashboard.setValues(cmd_msg);
  displayManager.render(&screen);
}
