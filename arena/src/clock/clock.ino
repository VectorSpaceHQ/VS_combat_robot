#include <Adafruit_Protomatter.h>
#include "fonts/FreeSerif12pt7b.h"
#include "fonts/FreeSerif9pt7b.h"
#include "fonts/Picopixel.h"
#include <SimpleCLI.h>
#include <esp_now.h>
#include <WiFi.h>
#include "common.h"
#include "sender.h"


#define COUNTDOWN_DURATION 3
#define RUNNING_DURATION 20
#define CLOCK_LEFT_MARGIN 4

// #define BLACK    0x0000
// #define BLUE     0x001F
// #define RED      0xF800
// #define GREEN    0x07E0
// #define CYAN     0x07FF
// #define MAGENTA  0xF81F
// #define YELLOW   0xFFE0 
// #define WHITE    0xFFFF

uint8_t rgbPins[]  = {42, 41, 40, 38, 39, 37};
uint8_t addrPins[] = {45, 36, 48, 35, 21};
uint8_t clockPin   = 2;
uint8_t latchPin   = 47;
uint8_t oePin      = 14;

Sender player1;
Sender player2;
Sender judges;
clock_state state;//the current state of the clock (primary state machine)
long state_change_time;// millis time when the state changed
int timer_value;//value in seconds used for timer display or countdown display
bool redraw_clock;

sender_message recent_message;//populated by esp_now event handler

Adafruit_Protomatter matrix(
  64, 4, 1, rgbPins, 4, addrPins, clockPin, latchPin, oePin, false);

SimpleCLI cli;
Command getMacCommand;
Command setMessageCommand;
Command setStateCommand;

void change_state(clock_state new_state, bool force = false);//forward declare because of looping dependency

void setup(void) {
  Serial.begin(115200);

  // Initialize player handlers
  player1.begin('B', 0, 0, 0x001F);   //BLUE TEAM
  player2.begin('G', 0, 11, 0x07E0);  //GREEN TEAM
  judges.begin('J', 0, 22, 0xFFFF);   //WHITE JUDGE

  // Initialize led matrix...
  ProtomatterStatus status = matrix.begin();
  matrix.setTextWrap(false);

  // Initialize ESPNOW
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(handle_espnow_received);

  //initialize CLI
  getMacCommand = cli.addCmd("getm/ac",&handle_getMac_command);
  setMessageCommand = cli.addCmd("setM/essage", &handle_setMessage_command);
  setMessageCommand.addPosArg("sender");
  setMessageCommand.addPosArg("message");
  setStateCommand = cli.addCmd("setS/tate", &handle_setState_command);
  setStateCommand.addPosArg("state");

  change_state(STATE_ENDED, true);
}

void loop(void) {

  if (Serial.available()) {
    cli.parse(Serial.readStringUntil('\n'));
  }

  loop_state();

  if(redraw_clock)
  {
    draw_clock(matrix);
  }

  bool redraw = redraw_clock;
  redraw_clock = false;
  redraw |= player1.loop(matrix);
  redraw |= player2.loop(matrix);
  redraw |= judges.loop(matrix);

  if(redraw){
    matrix.show();
  }
}

void loop_state()
{
  switch(state)
  {
    case STATE_ENDED://transition out handled by admin cancel message
      break;

    case STATE_WAITING:
      if(player1.is_ready() && player2.is_ready() && judges.is_ready())
      {
        change_state(STATE_READY);//will re-call this function
      }
      break;

    case STATE_READY://transition out handled by MC ready message
      break;

    case STATE_COUNTDOWN:
      recalculate_timer(COUNTDOWN_DURATION);
      if(timer_value == 0)
      {
        change_state(STATE_RUNNING);//will re-call this function
      }
      break; 

    case STATE_RUNNING:
      recalculate_timer(RUNNING_DURATION);
      if(timer_value == 0)
      {
        change_state(STATE_ENDED);//will re-call this function
      }
      break;
  }
}


void handle_admin_message(sender_message message)
{
  switch(message.message)
  {
    case MESSAGE_CANCEL:
      if(state == STATE_ENDED)
      {
        change_state(STATE_WAITING);
      } else if(state == STATE_COUNTDOWN)
      {
        change_state(STATE_READY);
      }
      break;
    case MESSAGE_FORCESTOP:
      change_state(STATE_ENDED);
      break;
    case MESSAGE_FORCESTART:
      change_state(STATE_COUNTDOWN);
      break;
  }
}


void handle_message_received(sender_message message)
{
  Serial.printf("message received from %d: %d\n",message.sender, message.message);

  switch(message.sender)
  {
    case SENDER_P1:
      player1.handle_message_received(message);
      break;
    case SENDER_P2:
      player2.handle_message_received(message);
      break;
    case SENDER_JUDGE:
      judges.handle_message_received(message);
      handle_admin_message(message);
      break;
    case SENDER_MC:
      handle_admin_message(message);
      if(message.message == MESSAGE_READY && state == STATE_READY)
      {
        change_state(STATE_COUNTDOWN);
      }
      break;
  }
}


void handle_espnow_received(const uint8_t * mac_addr, const uint8_t *incomingData, int len)
{
  if(len == sizeof(recent_message))
  {
    memcpy(&recent_message, incomingData, sizeof(recent_message));
    handle_message_received(recent_message);
  }
}

void draw_clock(Adafruit_Protomatter &matrix)//return true if redraw required
{
matrix.fillRect(SENDER_UI_WIDTH+CLOCK_LEFT_MARGIN, 0, matrix.width()-(SENDER_UI_WIDTH+CLOCK_LEFT_MARGIN), 32, 0);//set black background

  matrix.setTextColor(0xFFFF);//white as default
  matrix.setFont(&FreeSerif12pt7b);//default big font
  matrix.setCursor(SENDER_UI_WIDTH+CLOCK_LEFT_MARGIN,23);//default spacing for big font
  matrix.setTextSize(1);

  switch(state)
  {
    case STATE_ENDED:
      matrix.setTextColor(0xF800); //RED
      matrix.print("0:00");
      break;

    case STATE_WAITING:
      matrix.setTextColor(0xFFFF); //WHITE
      matrix.setFont(&Picopixel);
      matrix.setTextSize(2);
      matrix.setCursor(SENDER_UI_WIDTH+CLOCK_LEFT_MARGIN,19);
      matrix.print("READY?");
      break;

    case STATE_READY:
      matrix.setTextColor(0x07E0); //GREEN
      matrix.setFont(&Picopixel);
      matrix.setTextSize(2);
      matrix.setCursor(SENDER_UI_WIDTH+CLOCK_LEFT_MARGIN,19);
      matrix.print("READY!");
      break;

    case STATE_COUNTDOWN:
      matrix.setTextColor(0xFFE0); //YELLOW
      matrix.setCursor(SENDER_UI_WIDTH+CLOCK_LEFT_MARGIN+16,23);
      matrix.print(timer_value);
      break; 

    case STATE_RUNNING:
      if(timer_value <= 10)//for the final 10 seconds of the match change colors
      {
        matrix.setTextColor(0xFFE0); //YELLOW
      }
      matrix.printf("%d:%02d",timer_value/60,timer_value%60);
      break;
  }
  
}

void recalculate_timer(int starting_value)
{
  int new_timer_value = constrain(starting_value - (millis()-state_change_time)/1000,0,starting_value);
  if(new_timer_value != timer_value)
  {
    timer_value = new_timer_value;
    redraw_clock = true;
  }
}

void change_state(clock_state new_state, bool force)
{
  if(new_state != state || force)
  {
    Serial.printf("Changing state to %d\n",new_state);
    state = new_state;
    state_change_time = millis();
    redraw_clock = true;
    player1.handle_state_change(state);
    player2.handle_state_change(state);
    judges.handle_state_change(state);
    loop_state();
  }
}

void handle_getMac_command(cmd* c)
{
  Serial.println(WiFi.macAddress());
}

void handle_setMessage_command(cmd* c)
{
  Command cmd = Command(c);

  sender_message message;
  if(String("P1").equalsIgnoreCase(cmd.getArgument("sender").getValue()))
  {
    message.sender = SENDER_P1;
  } else if(String("P2").equalsIgnoreCase(cmd.getArgument("sender").getValue()))
  {
    message.sender = SENDER_P2;
  } else if(String("JUDGE").equalsIgnoreCase(cmd.getArgument("sender").getValue()))
  {
    message.sender = SENDER_JUDGE;
  } else if(String("MC").equalsIgnoreCase(cmd.getArgument("sender").getValue()))
  {
    message.sender = SENDER_MC;
  } else {
    return;
  }

  if(String("READY").equalsIgnoreCase(cmd.getArgument("message").getValue()))
  {
    message.message = MESSAGE_READY;
  } else if(String("CANCEL").equalsIgnoreCase(cmd.getArgument("message").getValue()))
  {
    message.message = MESSAGE_CANCEL;
  } else if(String("STOP").equalsIgnoreCase(cmd.getArgument("message").getValue()))
  {
    message.message = MESSAGE_FORCESTOP;
  } else if(String("START").equalsIgnoreCase(cmd.getArgument("message").getValue()))
  {
    message.message = MESSAGE_FORCESTART;
  } else {
    return;
  }

  handle_message_received(message);
}

void handle_setState_command(cmd* c)
{
  Command cmd = Command(c);
  if(String("ENDED").equalsIgnoreCase(cmd.getArgument("state").getValue()))
  {
    change_state(STATE_ENDED,true);
  } else if(String("WAITING").equalsIgnoreCase(cmd.getArgument("state").getValue()))
  {
    change_state(STATE_WAITING,true);
  } else if(String("READY").equalsIgnoreCase(cmd.getArgument("state").getValue()))
  {
    change_state(STATE_READY,true);
  } else if(String("COUNTDOWN").equalsIgnoreCase(cmd.getArgument("state").getValue()))
  {
    change_state(STATE_COUNTDOWN,true);
  } else if(String("RUNNING").equalsIgnoreCase(cmd.getArgument("state").getValue()))
  {
    change_state(STATE_RUNNING,true);
  }
}
