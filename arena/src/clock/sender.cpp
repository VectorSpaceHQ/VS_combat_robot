#include "Arduino.h"
#include <Adafruit_Protomatter.h>
#include "common.h"
#include "fonts/Picopixel.h"
#include "sender.h"

Sender::Sender()
{
  _id = ' ';
  _x = 0;
  _y = 0;
  has_changed = true;
  _status = SENDER_CLEARED;
  _state = STATE_ENDED;
  message_update_time = millis();
}

void Sender::begin(char id, uint x, uint y, uint8_t text_color)
{
  _id = id;
  _x = x;
  _y = y;
  _text_color = text_color;
  has_changed = true;
}

bool Sender::loop(Adafruit_Protomatter &matrix)
{
  if(_status == SENDER_CANCELED && millis() > (message_update_time + CANCEL_DELAY))
  {
    _status = SENDER_CLEARED;
    has_changed = true;
  }

  if(has_changed)
  {
    draw(matrix);
    has_changed = false;
    return true;
  } else {
    return false;
  }

  
}

void Sender::handle_message_received(sender_message message)
{
  if(_state == STATE_WAITING || _state == STATE_READY)
  {
    if(message.message == MESSAGE_READY && _status != SENDER_READY)
    {
      _status = SENDER_READY;
      message_update_time = millis();
      has_changed = true;
    } else if(message.message == MESSAGE_CANCEL && _status == SENDER_READY)
    {
      _status = SENDER_CANCELED;
      message_update_time = millis();
      has_changed = true;
    }
  }
}

void Sender::handle_state_change(clock_state state)
{
  if(state != _state)
  {
    _state = state;
    has_changed = true;
    if(_state != STATE_WAITING && _state != STATE_READY && _state != STATE_COUNTDOWN)
    {
      _status = SENDER_CLEARED;
    }
  }
}

bool Sender::is_ready()
{
  return _status == SENDER_READY;
}

void Sender::draw(Adafruit_Protomatter &matrix)
{
  uint16_t fill_color;
  switch(_status)
  {
    case SENDER_READY:
      fill_color = SENDER_READY_COLOR;
      break;
    case SENDER_CANCELED:
      fill_color = SENDER_CANCEL_COLOR;
      break;
    default:
      fill_color = SENDER_DEFAULT_COLOR;
  }

  matrix.fillRect(_x,_y,SENDER_UI_WIDTH,SENDER_UI_HEIGHT,fill_color);
  matrix.setFont(NULL);
  matrix.setTextSize(1);
  matrix.setCursor(_x + SENDER_UI_WIDTH/2 - 3, _y + SENDER_UI_HEIGHT/2 + -3);
  matrix.setTextColor(_status==SENDER_CLEARED?_text_color:SENDER_HIGHLIGHT_TEXT_COLOR);
  matrix.print(_id);
}