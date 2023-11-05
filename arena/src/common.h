#pragma once

#include "Arduino.h"

enum clock_state : uint8_t
{
  STATE_ENDED,             //BOOT. no timer and no signals from senders. proceedes to WAITING when CANCEL is received from judges or MC.
  STATE_WAITING,          //proceedes to READY when P1, P2, and judges are all ready
  STATE_READY,            //proceedes to COUNTDOWN when READY is received from MC
  STATE_COUNTDOWN,        //proceedes to RUNNING after the 3 second countdown is displayed. can be cancelled by judges or MC -> goes back to waiting
  STATE_RUNNING,          //the timer displays the clock for the match, which counts down to zero. proceedes to ENDED when time elapses (or a FORCESTOP is received.)
  
};

enum message_type : uint8_t
{
  MESSAGE_KEEPALIVE,      //does not effect clock state but reminds the clock that this sender is still connected
  MESSAGE_READY,          //sets this sender's state to ready
  MESSAGE_CANCEL,         //if in WAITING, unsets the ready status of the sender. If in COUNTDOWN, and sent from judges or MC, revers to WAITING.
  MESSAGE_FORCESTOP,      //forces to STATE_ENDED. only valid from judges or MC
  MESSAGE_FORCESTART      //forces to STATE_COUNTDOWN. only valid from judges or MC
};

enum sender_type : uint8_t 
{
  SENDER_UNKNOWN,         //for when the EEPROM is not set yet. messages from this sender are ignored
  SENDER_P1,              //player 1
  SENDER_P2,              //player 2
  SENDER_JUDGE,           //judges
  SENDER_MC               //MC
};

enum sender_status : uint8_t
{
  SENDER_CLEARED,         //initial status set at the beginning of WAITING
  SENDER_READY,           //if MESSAGE_READY is sent during WAITING the sender is set to ready
  SENDER_CANCELED         //if MESSAGE_CANCEL is sent during WIATING the sender is set to canceled. this auto transitions to cleared after a few seconds
};

struct sender_message
{
  sender_type sender;
  message_type message;
};