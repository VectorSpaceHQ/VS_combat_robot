/*
 * Vector Space Combat Robot Receiver
 * XIAO ESP32-C3
 * 11/2023
*/
#include <Arduino.h>
#include "hardware.h"
#include "common.h"
#include "drive_motor.h"
#include "weapon.h"
#include "comms.h"
#include "diagnostics.h"
#include <WiFi.h>
#include <esp_now.h>
#include <PreferencesCLI.h> // by Andrew Burks
//#include <Servo.h> // ESP32 ESP32S2 AnalogWrite by David Lloyd
//#include "cli.h"

#define SOFTWARE_VERSION "v1.2"
#define CONNECTION_TIMEOUT_MS 1000

//globals from libraries
Preferences preferences;
SimpleCLI cli;
PreferencesCLI prefCli(preferences);

//globals for libraries
DriveMotor leftMotor;
DriveMotor rightMotor;
Diagnostics diagnostics;
Weapon weapon(D5);

//globals for receiver states
ReceiverState currentState = RECEIVER_STATE_BOOT;
ReceiverFault currentFaults = RECEIVER_FAULT_NONE;
ReceiverWarning currentWarnings = RECEIVER_WARNING_NONE;

//globals for CLI
char cliResponseBuffer[256];
Command helpCommand;//command used to display CLI help
Command restartCommand;//command used to restart the transmitter or receiver
Command getVarCommand;//used to return values of certain internal variables

//globals for communication
/* esp_now_peer_info_t transmitterCommsInfo; */
CommandMessage cmd_msg;              //incoming
ResponseMessage rsp_msg;            //outgoing









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
  else if (variableName.equalsIgnoreCase("weaponSpeed")) Serial.println(cmd_msg.weapon_speed);
  else Serial.println("Variable is not supported by this command");
}


bool cliSetup()
{
  cli.setErrorCallback(cliErrorCallback);

  helpCommand = cli.addCommand("help", helpCommandCallback);
  helpCommand.setDescription("Use to display details on how to use a specific command\r\nAvailable Commands:\r\n\tsetPreference (setp)\r\n\tgetPreference (getp)\r\n\tclearPreference (clearp)\r\n\trestart\r\n\tgetVariable");
  helpCommand.addPositionalArgument("commandName,cmd","");

  prefCli.registerCommands(cli);

  getVarCommand = cli.addCommand("getV/ar/iable,getv", getVariableCommandCallback);
  getVarCommand.setDescription("Used to get the current value of the specified variable");
  getVarCommand.addPositionalArgument("v/ar/iable");

  return true;
}




void setup() {
  currentState = RECEIVER_STATE_STARTUP;
  bool startupOK = true;

  Serial.begin(115200);
  Serial.println("Vector Space {Combat Robot}");
  Serial.println(SOFTWARE_VERSION);
  Serial.println("https://github.com/VectorSpaceHQ/VS_combat_robot\r\n");

  startupOK &= diagnostics.setup();
  startupOK &= cliSetup();
  startupOK &= leftMotor.init(PIN_LEFT_MOTOR_FORWARD, PIN_LEFT_MOTOR_BACKWARD,
                              LEDC_TIMER_2, LEDC_CHANNEL_2, LEDC_CHANNEL_3);
  startupOK &= rightMotor.init(PIN_RIGHT_MOTOR_FORWARD, PIN_RIGHT_MOTOR_BACKWARD,
                               LEDC_TIMER_2, LEDC_CHANNEL_0, LEDC_CHANNEL_1);
  startupOK &= weapon.setup();
  startupOK &= espNowSetup(preferences);
  

  delay(200);
  weapon.arm();

  if(!startupOK)
  {
    currentState = RECEIVER_STATE_CRITICAL_FAULT;
    Serial.println("ERROR: Critical fault during startup is preventing transition to normal operation");
  } else {
    currentState = RECEIVER_STATE_CONNECTING;
    Serial.println("Startup Successful, transitioning to connection state");
  }
  Serial.println();
  Serial.print("> ");
}

void loop(){
  cmd_msg = GetCommandMessage();
  rsp_msg = GetResponseMessage();

  leftMotor.loop(cmd_msg.left_speed, currentState == RECEIVER_STATE_OPERATION);
  rightMotor.loop(cmd_msg.right_speed, currentState == RECEIVER_STATE_OPERATION);
  weapon.loop(cmd_msg.weapon_speed, currentState == RECEIVER_STATE_OPERATION);
  diagnostics.loop(currentState);

  if(currentState == RECEIVER_STATE_CONNECTING)
  {
    if(cmd_msg.id != rsp_msg.command_id)
    {
      currentState = RECEIVER_STATE_OPERATION;
      Serial.println("Command Message Received, transitioning to normal operation");
      /* weapon.arm(); */ // need to implement non-blocking before arming here
    }
  } else if (currentState == RECEIVER_STATE_OPERATION)
  {
    if(cmd_msg.id != rsp_msg.command_id)
    {
      sendResponse(currentState, currentFaults, currentWarnings);
    } else if((rsp_msg.uptime + CONNECTION_TIMEOUT_MS) < millis())
    {
      currentState = RECEIVER_STATE_CONNECTING;
      Serial.println("ERROR: Command message timeout, transitioning to connecting state");
      weapon.disarm();
    }
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

  delay(10); // not sure if this is necessary
}