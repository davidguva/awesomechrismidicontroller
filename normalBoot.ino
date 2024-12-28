void normalSetup() {
       MIDI.begin();
  USB.begin();
}

void normalLoop() {
  // processJoystickData(sendJoystickDataUSB);
  bool buttonPressed = false;

  for (int i = 0; i < num_pins; i++) {
    debouncers[i].update();  // Update the debouncers

    // Check if any button was just pressed
    if (debouncers[i].fell()) {
      buttonPressed = true;
      // Call the function with the index of the button pressed (starting from 1)
      sendButtonDown(i);
    } else if (debouncers[i].rose()) {
      sendButtonUp(i);
    }
  }
}

void sendButtonDown(int buttonIndex) {
  // Access the button array from the JSON
  JsonArray buttons = settings["settings"]["buttons"].as<JsonArray>();

  if (buttons.size() > buttonIndex) {
    JsonArray button = buttons[buttonIndex];  // Get the button settings array

    int type = button[0];      // Type of MIDI message (1 = Control Change, 2 = Program Change, 3 = Note On)
    int value = button[1];     // Value (Control number or Note number or Program number)
    int velocity = button[2];  // Velocity or Control value (for Control Change and Note On)

    // Handle based on the type of MIDI message
    switch (type) {
      case 1:  // Control Change
        Serial.println("sending control change");
        Serial.println(value);
        MIDI.controlChange(value, velocity, 1);  // Use channel 1 for now
        break;
      case 2:                          // Program Change
        MIDI.programChange(value, 1);  // Send Program Change on channel 1
        break;
      case 3:                             // Note On
        MIDI.noteOn(value, velocity, 1);  // Send Note On with velocity on channel 1
        break;
      default:
        Serial.println("Unknown MIDI message type");
        break;
    }
  }
}

void sendButtonUp(int buttonIndex) {
  // Access the button array from the JSON
  JsonArray buttons = settings["settings"]["buttons"].as<JsonArray>();
  if (buttons.size() > buttonIndex) {
    JsonArray button = buttons[buttonIndex];  // Get the button settings array
    int type = button[0];                     // Type of MIDI message (1 = Control Change, 3 = Note On)
    int value = button[1];                    // Value (Control number or Note number)

    // Handle based on the type of MIDI message
    switch (type) {
      case 1:                             // Control Change
        MIDI.controlChange(value, 0, 1);  // Send value 0 for button release (Control Change)
        break;
      case 2:  // Program Change
        // Program changes typically don’t require an "up" event
        break;
      case 3:                       // Note On -> Send Note Off when button is released
        MIDI.noteOff(value, 0, 1);  // Send Note Off with velocity 0 on channel 1
        break;
      default:
        Serial.println("Unknown MIDI message type");
        break;
    }
  }
}
