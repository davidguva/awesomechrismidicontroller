# awesome chris midi controller

### PLEASE NOTE THAT THIS IS A WORK IN PROGRESS PROJECT.

The code works and has been tested but it's not pretty.

---
This project is an advanced MIDI controller built using an ESP32 board. It features:
- Button inputs with debouncing for reliable operation.
- Configurable modes including Normal, Config, and Bluetooth (BT) modes.
- Persistent settings stored in LittleFS.
- Wi-Fi support for configuration via a web server.

---

### Known issues:
BLEMidi is dependent on NimBLE-Arduino which has been updated to version 2 and that will break the code. If you are using Arduino IDE please downgrade the library to 1.4.3 for now.

---
## Features
- **MIDI Support**: MIDI messages are sent via USB or Bluetooth, enabling communication with DAWs or MIDI-enabled devices.
- **Multiple Modes**:
  - **Normal Mode**: Default operating mode.
  - **Config Mode**: Allows users to configure settings via a web interface.
  - **BT Mode**: Enables Bluetooth MIDI functionality.
- **Persistent Settings**: Wi-Fi credentials and joystick configurations are stored in LittleFS.
---
## Planned features
- Joystick inputs for smooth control. Which are implemented in the current code but not yet tested.
- Add more configurable options for joystick and button behavior.
- Implement advanced LED animations for feedback.
---
## Hardware Requirements
- ESP32 board with sufficient GPIO pins and with support for MIDI over USB. I would suggest the ESP32-S3 which is cheap and has enough inputs for at least a 12 button pedal.
- Buttons for input (connected to specified GPIO pins).
- Joystick (connected to analog pins for X and Y axes). (Obviously not required for now)
- LEDs (optional for status indication). (Optional)
---
## Software Requirements

- **Libraries**:
  - [Bounce2](https://github.com/thomasfredericks/Bounce2): For button debouncing.
  - [LittleFS](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html): For file system support.
  - [ArduinoJson](https://arduinojson.org/): For parsing configuration files.
  - [USBMIDI](https://www.arduino.cc/): For USB MIDI communication.
  - [WiFi](https://www.arduino.cc/): For Wi-Fi functionality.
  - [WebServer](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer): To host the configuration interface.
  - [BLEMidi](https://github.com/asterics/Arduino-BLE-MIDI): For Bluetooth MIDI.

---

## Configuration
###### 1.	Normal Mode:
Default if no button is pressed during boot.
###### Config Mode:
Entered by holding the config button (gpio_pins[1]) during boot.
Enables configuration via Wi-Fi and web interface.
###### BT Mode:
Entered by holding the Bluetooth button (gpio_pins[0]) during boot.
Enables Bluetooth MIDI functionality.

The JSON structure for each button is   [1, 1, 127] which translates to  [type, note, velocity]. The types are 1. CC, 2. PC and 3. Note.

---

### License
This project is open-source and distributed under the MIT License.
