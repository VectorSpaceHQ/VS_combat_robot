# Hardware
MCU: ESP32-C3
max supply voltage 3.7V

Voltage Regulator: AMS1117-3.3
Max supply voltage 15V. Regulates to 3.3V

Motor Drivers: DRV8873
Vin: 6.5V to 45V
3.6A max

# Software
## Receiver

## Transmitter
### Installation
#### Determine MAC Address of ESP32
1. Upload MAC_finder.ino using Arduino IDE
2. Look at Serial Monitor and copy down resulting MAC address
3. Copy this MAC Address into transmitter.ino

#### Determine joystick rest values



# Troubleshooting
## ESP32 Error: No serial data received

1. Open the Arduino IDE Serial Monitor
2. Hold-down the “BOOT” button
3. Press the “ENABLE” button once
4. Release the finger from the “BOOT” button


## ESP32 Error: A serial exception error occurred: Write timeout
 Press and hold the R button, then press and hold the B button, then release the R button, then release the B button.
