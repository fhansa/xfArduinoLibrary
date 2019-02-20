/* xfArduinoLibrary - xfOTA
 * Copyright Fredrik Santander 2019
 * MIT License
 *
 * This example shows how to use xfOTA. 
 * 
 */
#include <ESP8266WiFi.h>
#include <xfOTA.h>

xfOTA ota;

void setup() {
  // First we need wifi 
  WiFi.begin("YOUR SSID", "YOUR PASSWORD");
  WiFi.config(IPAddress(192.168.1.3), IPAddress(192.168.1.1), IPAddress(255.255.255.0));
  while ((WiFi.status() != WL_CONNECTED) ) {
    delay(10);
  }
  Serial.println("Wifi ok");

  // Setup OTA
  ota.setup("ESP8266", "otapassword");
}

void loop() {
  // Check for OTA connection as often as you can
  ota.loop();
}