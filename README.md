This is an RC controller designed for use in combat robotics. It includes PCB designs for the transmitter and receiver, as well as the necessary Arduino code. It's inspired by the work of Team Panic (https://www.instructables.com/id/Cheap-Arduino-Combat-Robot-Control/), and developed after building and using 12 of their PCBs.

Below are the changes
* Added on/off switch
* Two joysticks, as one is difficult to mix and control. They're also extremely cheap.
* Joysticks connect to female headers rather than being soldered directly. Easily replacable and easier to install.
* Uses a 3.3V Arduino Pro Mini instead of 5V
* Voltage regulator goes directly to VCC and VCC ties to NRF24. This allows powering both components through the USB programmer, no longer requiring a battery to be plugged in for testing.
* Added an LED for active communications indication
* Weapon is now an on/off switch
* Receiver has pads for the motor control signals


# PCB
The PCBs are designed using KiCAD and require the Arduino Pro Mini library (https://github.com/Duckle29/kicad-libraries). To produce the PCBs, zip the gerber directories and upload to a PCB manufacturer website.

# Arduino Code
