void btSetup() {
  BLEMidiServer.begin("Cool Midi Pedal v02");
}

void btLoop() {
  // processJoystickData(sendJoystickDataBT);

  bool buttonPressed = false;

  for (int i = 0; i < num_pins; i++) {
    debouncers[i].update();  // Update the debouncers

    // Check if any button was just pressed
    if (debouncers[i].fell()) {
      buttonPressed = true;

      // Call the function with the index of the button pressed (starting from 1)
      sendButtonDownBT(i);
    } else if (debouncers[i].rose()) {
      sendButtonUpBT(i);
    }
  }
}

void sendButtonDownBT(int buttonIndex) {
  JsonArray buttons = settings["settings"]["buttons"].as<JsonArray>();

  if (buttons.size() > buttonIndex) {
    JsonArray button = buttons[buttonIndex];  // Get the button settings array

    int type = button[0];           // Type of MIDI message (1 = Control Change, 2 = Program Change, 3 = Note On)
    int value = button[1];          // Value (Control number or Note number or Program number)
    int velocity = button[2];       // Velocity or Control value (for Control Change and Note On)

    // Handle based on the type of MIDI message
    switch (type) {
      case 1:  // Control Change
        BLEMidiServer.controlChange(1, value, velocity);
        break;
      case 2:  // Program Change
        BLEMidiServer.programChange(1, value);
        break;
      case 3:  // Note On
        BLEMidiServer.noteOn(1, value, velocity);
        break;
      default:
        Serial.println("Unknown MIDI message type");
        break;
    }
  }
}

void sendButtonUpBT(int buttonIndex) {
   JsonArray buttons = settings["settings"]["buttons"].as<JsonArray>();
  if (buttons.size() > buttonIndex) {
    JsonArray button = buttons[buttonIndex];  // Get the button settings array
    int type = button[0];                     // Type of MIDI message (1 = Control Change, 3 = Note On)
    int value = button[1];                    // Value (Control number or Note number)

    // Handle based on the type of MIDI message
    switch (type) {
      case 1:                             // Control Change
        BLEMidiServer.controlChange(1, value, 0);
        break;
      case 2:  // Program Change
        // Program changes typically don’t require an "up" event
        break;
      case 3:                       // Note On -> Send Note Off when button is released
        BLEMidiServer.noteOff(1, value, 0);
        break;
      default:
        Serial.println("Unknown MIDI message type");
        break;
    }
  }
}