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
#include "diagnostics.h"
#include "UiUiUi.h" //DOWNLOAD FROM HERE BECAUSE IT HAS FIXES NOT IN ARDUINO VERSION: https://github.com/burksbuilds/UiUiUi
#include "UIStatusIndicator.h"
#include "UISidebar.h"
#include "UIDashboard.h"
#include "UIMessageBanner.h"
#include "wifi_comms.h"
#include "cli.h"
#include "auto_pairing.h"

#define SOFTWARE_VERSION "v2"

#define CONNECTION_TIMEOUT_MS 1000

//globals for transmitter states
TransmitterState currentState = TRANSMITTER_STATE_BOOT;
TransmitterFault currentFaults = TRANSMITTER_FAULT_NONE;
TransmitterWarning currentWarnings = TRANSMITTER_WARNING_NONE;

//globals for communication
//esp_now_peer_info_t receiverCommsInfo;
CommandMessage cmd_msg;          //outgoing
ResponseMessage rsp_msg;        //incoming

//globals for hardware
PairButton pairButton(PIN_LEFT_TRIGGER, PIN_RIGHT_TRIGGER, PIN_RIGHT_THUMB_SWITCH);
Joystick leftJoystick;
Joystick rightJoystick;


Diagnostics diagnostics;
LED faultLED(PIN_FAULT_LED);
LED commsLED(PIN_COMMS_LED);

uint16_t batteryStateOfCharge;
float batteryVoltage;
float batteryVoltageMultiplier;
float batteryVoltageLowRange;
float batteryVoltageHighRange;
float batteryVoltageAlarmLimit;

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
  commsLED.toggle();
  delay(1000);
  commsLED.toggle();
  delay(1000);
  // -------- -----

  currentState = TRANSMITTER_STATE_STARTUP;
  bool startupOK = true;

  Serial.begin(115200);
  Serial.println("Vector Space {Combat Robot}");
  Serial.println(SOFTWARE_VERSION);
  Serial.println("https://github.com/VectorSpaceHQ/VS_combat_robot\r\n");

  startupOK &= leftJoystick.setup(PIN_LEFT_JOYSTICK,"LeftJoystick", true);
  startupOK &= rightJoystick.setup(PIN_RIGHT_JOYSTICK,"RightJoystick", true);

  pinMode(PIN_WEAPON_TOGGLE_SWITCH,INPUT_PULLUP);
  pinMode(PIN_RIGHT_THUMB_SWITCH,INPUT_PULLUP);
  pinMode(PIN_RIGHT_TRIGGER,INPUT_PULLUP);
  pinMode(PIN_LEFT_TRIGGER,INPUT_PULLUP);
  pinMode(PIN_BATTERY_SENSE,INPUT_PULLUP);
  batteryVoltage = batteryVoltageMultiplier * (analogReadMilliVolts(PIN_BATTERY_SENSE) / 1000.0);

  /* pinMode(PIN_FAULT_LED,OUTPUT);
  digitalWrite(PIN_FAULT_LED,LOW);
  pinMode(PIN_COMMS_LED,OUTPUT);
  digitalWrite(PIN_COMMS_LED,LOW); */
  faultLED.off();
  commsLED.off();

  Preferences preferences;
  preferences.begin("Battery");
  batteryVoltageMultiplier = preferences.getFloat("Multiplier",DEFAULT_BATTERY_VOLTAGE_MULTIPLIER);
  batteryVoltageLowRange = preferences.getFloat("LowRange",DEFAULT_BATTERY_LOW_RANGE_VOLTAGE);
  batteryVoltageHighRange = preferences.getFloat("HighRange",DEFAULT_BATTERY_HIGH_RANGE_VOLTAGE);
  batteryVoltageAlarmLimit = preferences.getFloat("AlarmLimit",DEFAULT_BATTERY_ALARM_LIMIT_VOLTAGE);
  preferences.end();

  startupOK &= screenSetup();
  startupOK &= espNowSetup();
  startupOK &= cliSetup();
  startupOK &= diagnostics.setup();
  PairSetup();

  if(!startupOK)
  {
    currentState = TRANSMITTER_STATE_CRITICAL_FAULT;
    Serial.println("ERROR: Critical fault during startup is preventing transition to normal operation");
    faultLED.on();
    controllerSidebar.updateStatusIndicator(UIStatusIcon::Fault);
    messageBanner.setMessage(UIStatusIcon::Fault,"Critical Fault","Unable to complete startup");
  } else {
    currentState = TRANSMITTER_STATE_CONNECTING;
    Serial.println("Startup Successful, transitioning to connecting state");
    controllerSidebar.updateStatusIndicator(UIStatusIcon::None);
    messageBanner.setMessage(UIStatusIcon::Info,"Startup Complete","Connecting to robot...");
    faultLED.off();
  }


    Serial.println();
    Serial.print("> ");
}

void loop() {
  cmd_msg = GetCommandMessage();
  rsp_msg = GetResponseMessage();

  pairButton.loop(&commsLED);

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

  ParseCLI();

  dashboard.setValues(cmd_msg);
  displayManager.render(&screen);
  diagnostics.loop(currentState, &commsLED, &faultLED);

}
