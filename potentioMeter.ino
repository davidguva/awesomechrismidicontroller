// Constants
const int deadZoneMin = 2000;     // Dead zone lower bound
const int deadZoneMax = 2100;     // Dead zone upper bound
const unsigned long deadZoneTimeout = 1000; // 1 second timeout for dead zone

// Smoothing variables

int readIndex = 0;               // Index for smoothing array
unsigned long lastMovementTime = 0; // Last time outside dead zone

void processJoystickData(void (*sendJoystickData)(int xValue, int yValue)) {
  // Read raw joystick values
  int rawXValue = analogRead(vrxPin);
  int rawYValue = analogRead(vryPin);

  // Update smoothing calculations for X axis
  totalX -= xReadings[readIndex];
  xReadings[readIndex] = rawXValue;
  totalX += xReadings[readIndex];
  smoothedX = totalX / smoothingFactor;

  // Update smoothing calculations for Y axis
  totalY -= yReadings[readIndex];
  yReadings[readIndex] = rawYValue;
  totalY += yReadings[readIndex];
  smoothedY = totalY / smoothingFactor;

  // Move to the next position
  readIndex = (readIndex + 1) % smoothingFactor;

  // Check for dead zones
  bool inDeadZone_X = (smoothedX >= deadZoneMin && smoothedX <= deadZoneMax);
  bool inDeadZone_Y = (smoothedY >= deadZoneMin && smoothedY <= deadZoneMax);

  if (inDeadZone_X && inDeadZone_Y) {
    // If in the dead zone, check timeout
    if (millis() - lastMovementTime > deadZoneTimeout) {
      return; // Do not send data
    }
  } else {
    // Update last movement time if outside dead zone
    lastMovementTime = millis();
  }

  // Map smoothed values and send data
  int midiCCValue_X = inDeadZone_X ? -1 : map(smoothedX, 0, 4095, 0, 127);
  int midiCCValue_Y = inDeadZone_Y ? -1 : map(smoothedY, 0, 4095, 0, 127);

  sendJoystickData(midiCCValue_X, midiCCValue_Y);
}

void sendJoystickDataUSB(int xValue, int yValue) {
  if (xValue != -1) {
    MIDI.controlChange(joyStickXCCnr, xValue, 1);  // Send X-axis CC
  }
  if (yValue != -1) {
    MIDI.controlChange(joyStickYCCnr, yValue, 1);  // Send Y-axis CC
  }
}

void sendJoystickDataBT(int xValue, int yValue) {
  if (xValue != -1) {
    BLEMidiServer.controlChange(0, joyStickXCCnr, xValue);  // Send X-axis CC
  }
  if (yValue != -1) {
    BLEMidiServer.controlChange(0, joyStickYCCnr, yValue);  // Send Y-axis CC
  }
}