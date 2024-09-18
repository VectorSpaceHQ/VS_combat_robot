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
#include "wifi_comms.h"
#include "diagnostics.h"
#include "cli.h"
#include "auto_pairing.h"
#include <WiFi.h>
#include <esp_now.h>
#include <PreferencesCLI.h> // by Andrew Burks
//#include <Servo.h> // ESP32 ESP32S2 AnalogWrite by David Lloyd


#define SOFTWARE_VERSION "v1.2"
#define CONNECTION_TIMEOUT_MS 1000


//globals for libraries
DriveMotor leftMotor;
DriveMotor rightMotor;
Diagnostics diagnostics;
Weapon weapon(D5);
PairButton pairButton(D9);
LED commsLED(PIN_COMMS_LED);
LED optionalLED(PIN_OPT_LED);

//globals for receiver states
ReceiverState currentState = RECEIVER_STATE_BOOT;
ReceiverFault currentFaults = RECEIVER_FAULT_NONE;
ReceiverWarning currentWarnings = RECEIVER_WARNING_NONE;

//globals for communication
/* esp_now_peer_info_t transmitterCommsInfo; */
CommandMessage cmd_msg;              //incoming
ResponseMessage rsp_msg;            //outgoing


void SetState(CommandMessage cmd_msg, ResponseMessage rsp_msg){
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
}


void setup() {
  currentState = RECEIVER_STATE_STARTUP;
  bool startupOK = true;

    Serial.begin(115200);
  /* while(!Serial){} // DEBUG */
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
  startupOK &= espNowSetup();
  

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
  diagnostics.loop(currentState, commsLED, optionalLED);

  SetState(cmd_msg, rsp_msg);
  ParseCLI(); // doesn't need to happen very often
  

  pairButton.loop(&commsLED); // Check status of pairbutton

  //Serial.println(cmd_msg.left_speed);

}