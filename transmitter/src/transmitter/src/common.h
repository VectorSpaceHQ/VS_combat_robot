#ifndef COMMON_H
#define COMMON_H

#define ENUM_NAME(name) #name

enum TransmitterState : uint8_t
{
	TRANSMITTER_STATE_BOOT,				//initial state, sets up logs/serial then transitions to startup
	TRANSMITTER_STATE_STARTUP,			//initializes all of the devices and configurations. transitions to connecting OR configuration when ready
	TRANSMITTER_STATE_PAIRING,
  TRANSMITTER_STATE_CONNECTING,  //attempting to get in touch with a receiver transitions to OPERATION once a packet makes it back
	TRANSMITTER_STATE_OPERATION,			//indefinite state. commands are transmitted cyclically. falls back to
	TRANSMITTER_STATE_CRITICAL_FAULT,		//transition from any state when something terrible happens. probably needs to be rebooted, but logs may have good info
	TRANSMITTER_STATE_CONFIGURATION		//a non-transmitting state meant for safe reconfiguration of settings. requires reboot to exit
};

enum TransmitterFault : uint16_t
{
	TRANSMITTER_FAULT_BATTERY_FAILURE 	= 1<<0,
	TRANSMITTER_FAULT_SCREEN_FAILURE		= 1<<1,
	TRANSMITTER_FAULT_SERIAL_FAILURE		= 1<<2,
	TRANSMITTER_FAULT_SETTINGS_FAILURE	= 1<<3,
	TRANSMITTER_FAULT_CONNECTION_FAILURE 	= 1<<4,
	TRANSMITTER_FAULT_NONE 				= 0
};

enum TransmitterWarning : uint16_t
{
	TRANSMITTER_WARNING_BATTERY_LOW 		= 1<<0,
	TRANSMITTER_WARNING_WIFI_SIGNAL_LOW		= 1<<1,
	TRANSMITTER_WARNING_ROUNDTRIP_TIMEOUT	= 1<<2,
	TRANSMITTER_WARNING_L_JOYSTICK_FAILURE	= 1<<3,
	TRANSMITTER_WARNING_R_JOYSTICK_FAILURE	= 1<<4,
	TRANSMITTER_WARNING_TRANSMIT_FAILURE 	= 1<<5,
	TRANSMITTER_WARNING_NONE 				= 0
};

enum ReceiverState : uint8_t
{
	RECEIVER_STATE_BOOT,		//initial state, sets up logs/serial then transitions to startup
	RECEIVER_STATE_STARTUP,		//initializes all of the devices and configurations. transitions to connecting when ready
	RECEIVER_STATE_PAIRING,
	RECEIVER_STATE_CONNECTING,	//tries to connect to (waits to receive) commands from the transmitter. transitions to operation when a command is received
	RECEIVER_STATE_OPERATION,	//indefinite state. falls back to connecting state if no commands received in a while
	RECEIVER_STATE_CRITICAL_FAULT	//transition from any state when something terrible happens. probably needs to be rebooted, but logs may have good info
};

enum ReceiverFault : uint16_t
{
	RECEIVER_FAULT_BATTERY_FAILURE 	= 1<<0,
	RECEIVER_FAULT_SERIAL_FAILURE		= 1<<2,
	RECEIVER_FAULT_SETTINGS_FAILURE	= 1<<3,
	RECEIVER_FAULT_CONNECTION_FAILURE 	= 1<<4,
	RECEIVER_FAULT_NONE 				= 0
};

enum ReceiverWarning : uint16_t
{
	RECEIVER_WARNING_BATTERY_LOW 		= 1<<0,
	RECEIVER_WARNING_WIFI_SIGNAL_LOW		= 1<<1,
	RECEIVER_WARNING_COMMAND_TIMEOUT		= 1<<2,
	RECEIVER_WARNING_L_DRIVE_FAILURE		= 1<<3,
	RECEIVER_WARNING_R_DRIVE_FAILURE		= 1<<4,
	RECEIVER_WARNING_TRANSMIT_FAILURE 	= 1<<5,
	RECEIVER_WARNING_WEAPON_FAILURE		= 1<<6,
	RECEIVER_WARNING_COMMAND_MISSED		= 1<<7,
	RECEIVER_WARNING_BOARD_VOLTAGE_LOW	= 1<<8,
	RECEIVER_WARNING_NONE 				= 0
};

enum RemoteCommand : uint16_t
{
  REMOTE_COMMAND_REBOOT           = 1<<0,
  REMOTE_COMMAND_NONE             = 0
};

struct CommandMessage
{
	uint32_t id;				//sequentially icnrease unique message identifier
	uint32_t send_time;			//uptime (milliseconds) of transmitter when message was sent
  RemoteCommand command;  //mask of commands that the receiver can execute
	int16_t left_speed;			//bidirectional desired speed of left drive (2^15-1 => 100% forward)
	int16_t right_speed;		//bidirectional desired speed of right drive (2^15-1 => 100% forward)
	int16_t weapon_speed;		//bidirectional desired speed of weapon drive (2^15-1 => 100% forward). unidirectional weapons ignore negatives
	int16_t servo_position; 	//bidirectional desired position of servo (2^15-1 => 100% forward). unidirectional servos ignore negatives
	uint16_t horn_frequency;	//desired frequency in Hz of the buzzer. 0 indicates no horn desired.
	ReceiverFault clear_faults;	//clear the indicated faults on the receiver
	ReceiverWarning clear_warnings;	//clear the indicated warnings on the receiver
};

struct ResponseMessage
{
    uint32_t command_id;		//id of the CommandMessage that prompted this response
    uint32_t uptime;        //current 'millis()' value on receiver
    ReceiverState state;		//current state of the receiver
    ReceiverFault faults;		//currently active faults of the receiver
    ReceiverWarning warnings;	//currently active warnings of the receiver
    uint16_t board_voltage;		//current voltage (millivolts) of the receiver board
    uint8_t battery_charge;		//current state of charge (0-100, as %) of theprimary receiver battery
    int16_t wifi_strength;		//RSSI of the indicated CommandMessage
    long responseMessageTime;
};


//hack to grab RSSI from the internals of the ESP32 code: https://github.com/gmag11/QuickESPNow/blob/7bb87e1715f3e81362b23e2872747c8e252507ed/src/QuickEspNow_esp32.cpp#L346
#define ESPNOW_RSSI_MEM_OFFSET 51
#define GET_ESPNOW_MESSAGE_RSSI(data_location) *((int8_t*)(data_location - ESPNOW_RSSI_MEM_OFFSET))



#endif
