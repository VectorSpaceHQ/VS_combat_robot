#pragma once

#include "Arduino.h"
#include <Adafruit_Protomatter.h>
#include "common.h"

#define SENDER_DEFAULT_COLOR 0x0000 //black
#define SENDER_READY_COLOR 0xFFFF //white
#define SENDER_CANCEL_COLOR 0xF800 //red
#define SENDER_HIGHLIGHT_TEXT_COLOR 0x0000 //black

#define SENDER_UI_HEIGHT 10
#define SENDER_UI_WIDTH 9
#define CANCEL_DELAY 2000

class Sender {
  public:
    Sender();
    void begin(char id, uint x, uint y, uint8_t text_color);//sets up 10Hx8W UI element with top left at position x/y with supplied single-character id ('1', '2', 'J')
    void handle_message_received(sender_message message);
    void handle_state_change(clock_state state);
    bool loop(Adafruit_Protomatter &matrix);//returns true if a screen update is required. responsible for drawing icon on led matrix
    bool is_ready();//true if sender status is ready

  private:
    char _id;
    uint _x;
    uint _y;
    uint8_t _text_color;
    bool has_changed;
    sender_status _status;
    clock_state _state;
    long message_update_time;//stores time since last message received (important for cancel)
    void draw(Adafruit_Protomatter &matrix);
};
