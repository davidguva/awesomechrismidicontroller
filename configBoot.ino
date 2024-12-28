// Serve the HTML page
void handleRoot() {
  File file = LittleFS.open("/index.html", "r");
  if (file) {
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(404, "text/plain", "File not found");
  }
}

// Serve the settings JSON file
void handleSettings() {
  String json;
  serializeJson(settings, json);
  server.send(200, "application/json", json);
}

// Handle POST request to update the settings
void handleSubmit() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    deserializeJson(settings, body);

    // Save updated settings to LittleFS
    saveSettings();

    String json;
    serializeJson(settings, json);
    server.send(200, "application/json", json);
  } else {
    server.send(400, "text/plain", "No data received");
  }
}

// Save settings to LittleFS
void saveSettings() {
  File file = LittleFS.open("/config.json", "w");
  if (file) {
    serializeJson(settings, file);
    file.close();
    Serial.println("Config saved");
  } else {
    Serial.println("Failed to open config.json for writing");
  }
}

void configSetup() {
  WiFi.mode(WIFI_AP);
  delay(250);
  // Start the Wi-Fi in Access Point mode
  WiFi.softAP(ssid, password);
      Serial.println("startar SSID");



  // Define server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/settings.json", HTTP_GET, handleSettings);
  server.on("/submit", HTTP_POST, handleSubmit);

  // Start the server
  server.begin();

}

void configLoop() {
  server.handleClient();
}