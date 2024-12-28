#include <Bounce2.h>
#include "USB.h"
#include "USBMIDI.h"
#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <BLEMidi.h>

WebServer server(80);

// Struct to define LED properties
struct LED {
  const char* name;              // LED name
  int pin;                       // Pin connected to the LED
  bool isBlinking;               // True if LED should blink
  unsigned long previousMillis;  // Last time LED state was toggled (for blinking)
  unsigned long endMillis;       // Time when LED should stop (for fixed duration)
  int interval;                  // Blink interval in ms
  int duration;                  // Duration in ms (how long the LED stays on/blinks), 0 = infinite
  bool isOn;                     // True if LED is turned on
  bool state;                    // True = HIGH, False = LOW (for blinking)
};


//37 , 38

USBMIDI MIDI;

//IMPORTANT VARIABLES THAT MIGHT NEED TO CHANGE
//___________________
#define bootThreshold 1000  // Time (in ms) to detect long press during boot

const int debounceTime = 5;

int gpio_pins[] = { 40, 39, 12, 38, 41, 42, 13, 14, 11 };  // Define the GPIO pins for the buttons

int vrxPin = 45;  // X-axis input for joystick
int vryPin = 48;  // Y-axis input for joystick

int num_pins = sizeof(gpio_pins) / sizeof(gpio_pins[0]);

// Create Bounce objects for each button
Bounce* debouncers = new Bounce[num_pins];

int smoothedX = 0;
int smoothedY = 0;
const int smoothingFactor = 20;   // Number of readings for smoothing
int xReadings[smoothingFactor] = {0};  // Initialize with zeros
int yReadings[smoothingFactor] = {0};  // Initialize with zeros
int totalX = 0, totalY = 0;      // Sums for smoothing

bool configMode = false;
bool btMode = false;
StaticJsonDocument<1024> settings;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup...");

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS");
    return; 
  }
  Serial.println("LittleFS mounted");

  // Load settings
  loadSettings();
  Serial.println("Settings loaded");

  // Initialize buttons
  for (int i = 0; i < num_pins; i++) {
    pinMode(gpio_pins[i], INPUT_PULLUP);
    debouncers[i].attach(gpio_pins[i]);
    debouncers[i].interval(debounceTime);
  }
  Serial.println("Buttons initialized");

  // Initialize joystick smoothing
  totalX = 0;
  totalY = 0;
  for (int i = 0; i < smoothingFactor; i++) {
    xReadings[i] = 0;
    yReadings[i] = 0;
    totalX += xReadings[i];
    totalY += yReadings[i];
  }
  Serial.println("Joystick smoothing initialized");


// Define button indices for specific modes
int configButtonIndex = 1;  // Change this to the button index for configMode
int btButtonIndex = 0;      // Change this to the button index for btMode

unsigned long startTime = millis();

while (millis() - startTime < bootThreshold) {
  debouncers[configButtonIndex].update();
  debouncers[btButtonIndex].update();
  
  // Check if config button is held
  if (debouncers[configButtonIndex].read() == LOW) {
    configMode = true;
    break;
  }
  // Check if BT mode button is held
  if (debouncers[btButtonIndex].read() == LOW) {
    btMode = true;
    break;
  }
}

// Setup based on the mode
if (configMode) {
  Serial.println("Config Mode");
  configSetup();  // Setup for Configuration 2
} else if (btMode) {
  Serial.println("BT Mode");
  btSetup();      // Setup for Bluetooth mode
} else {
  Serial.println("Normal Mode");
  normalSetup();  // Setup for Normal mode (Configuration 1)
}
  delay(1000);  // Boot delay
  Serial.println("Setup complete");
}

void loop() {
 // updateLEDs();
  if (configMode) {
    // Run the configuration mode loop
    configLoop();
  } else if (btMode) {
    // Run the Bluetooth mode loop
    btLoop();
  } else {
    // Run the normal mode loop
    normalLoop();
  }
}
// Variables to hold the Wi-Fi credentials
String ssid;
String password;
int joyStickXCCnr;
int joyStickYCCnr;

// Load settings from LittleFS
void loadSettings() {
  File file = LittleFS.open("/config.json", "r");
  if (!file) {
    Serial.println("config.json not found");
    return;
  }

  DeserializationError error = deserializeJson(settings, file);
  if (error) {
    Serial.print("Failed to parse config.json: ");
    Serial.println(error.c_str());
    return;
  }

  ssid = settings["settings"]["wifiSSID"] | "cool midi pedal";
  password = settings["settings"]["wifiPassword"] | "12345678";
  joyStickXCCnr = settings["settings"]["joyStickXCCnr"];
  joyStickYCCnr = settings["settings"]["joyStickYCCnr"];

  Serial.println("Config loaded");
}
