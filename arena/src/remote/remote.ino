#include <SimpleCLI.h>
#include <Preferences.h>
#include <PreferencesCLI.h>
#include <OneButton.h>
#include <esp_now.h>
#include <WiFi.h>
#include "common.h"

#define START_BUTTON_PIN D3
#define CANCEL_BUTTON_PIN D9
#define LED_PIN D7


sender_message message;//message to send to clock
esp_now_peer_info_t clock_comms_info;  


OneButton start_button = OneButton(START_BUTTON_PIN,true,true);
OneButton cancel_button = OneButton(CANCEL_BUTTON_PIN,true,true);

Preferences preferences;
SimpleCLI cli;
PreferencesCLI pcli(preferences);
Command sendMessageCommand;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  esp_now_init();

  start_button.attachClick(handle_start_button_click);
  start_button.attachLongPressStart(handle_start_button_hold);
  cancel_button.attachClick(handle_cancel_button_click);
  cancel_button.attachLongPressStart(handle_cancel_button_hold);

  preferences.begin("remote");
  message.sender = (sender_type)preferences.getUChar("sender",SENDER_UNKNOWN);
  if(preferences.isKey("address"))
  {
    preferences.getBytes("address",clock_comms_info.peer_addr,6);
    clock_comms_info.channel = 0;
    clock_comms_info.encrypt = false;
    esp_now_add_peer(&clock_comms_info);
  }  
  preferences.end();

  pcli.registerCommands(cli);
  sendMessageCommand = cli.addCmd("sendM/essage",handle_sendMessageCommand);
  sendMessageCommand.addPositionalArgument("message");

  Serial.print("> ");

  pinMode(LED_PIN,OUTPUT);
}

void loop() {
  if(Serial.available())
  {
    String cliInput = Serial.readStringUntil('\n');
    Serial.println(cliInput);
    cli.parse(cliInput);
    if(cli.available())
    {
      Command command = cli.getCommand();
      pcli.handleCommand(command,Serial);
    }
    Serial.println();
    Serial.print("> ");
  }

  start_button.tick();
  cancel_button.tick();

  digitalWrite(LED_PIN,HIGH);
}

void send_message(message_type type)
{
  Serial.printf("Sending message: %d\n",type);
  message.message = type;
  esp_now_send(clock_comms_info.peer_addr, (uint8_t*) &message, sizeof(message));
}

void handle_start_button_click()
{
  send_message(MESSAGE_READY);
}
void handle_start_button_hold()
{
  send_message(MESSAGE_FORCESTART);
}
void handle_cancel_button_click()
{
  send_message(MESSAGE_CANCEL);
}
void handle_cancel_button_hold()
{
  send_message(MESSAGE_FORCESTOP);
}

void handle_sendMessageCommand(cmd* c)
{
  Command cmd = Command(c);
  if(String("READY").equalsIgnoreCase(cmd.getArgument("message").getValue()))
  {
    send_message(MESSAGE_READY);
  } else if(String("CANCEL").equalsIgnoreCase(cmd.getArgument("message").getValue()))
  {
    send_message(MESSAGE_CANCEL);
  } else if(String("STOP").equalsIgnoreCase(cmd.getArgument("message").getValue()))
  {
    send_message(MESSAGE_FORCESTOP);
  } else if(String("START").equalsIgnoreCase(cmd.getArgument("message").getValue()))
  {
    send_message(MESSAGE_FORCESTART);
  } 
}