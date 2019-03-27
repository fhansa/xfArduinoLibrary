/*
 *  Build and test scetch used when developing the xfArduinoLibrary
 *  
 *  Validate test by reading serial output in conjunction with the 
 *  comments in the code. 
 * 
 *  Haven't found a better way to reliable test arduino library code 
 * 
 */
#include "xfArduinoLibrary.h"

// Set this to false to prevent settings from being deleted. 
// This is useful when testing other functions than WiFiManager
#define XFTEST_CLEARSETTINGS false

// Not part of example
// Need Wifi 
xfWifiManager wifiMgr("xftester", "update", true, true);

// Create binary sensor by giving base topic and device name
xfMQTTBinarySensor device("test", "XFTESTER");

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("TEST OF xfArduinoLibrary");

  // 1) Clear all settings
  Serial.println("1) *** Clearing settings");

  if (XFTEST_CLEARSETTINGS) {
    Serial.println("    Purging config files and reset Wifi");
    xfConfig.purgeFS();
    WiFi.disconnect();
  } else {
    Serial.println("    Settings not cleared (deactivated)");
  }
  // 2) Setup Wifi using wifiMagr including getting MQTT-properties
  Serial.println("2) *** Setup Wifi using xfWifiManager");
  wifiMgr.setupWifi();

  // 3) xfBinarySensor using wifiManager settings
  // 
  //  3a) Create device
  //  3b) Publish discovery
  //  3c) Publish state
  //  3d) Recieve notification

  // 4) xfConfig
  //  4a) Enable serializable struct and save values
  //  4b) Read values 

  Serial.println("**** END OF SETUP");
}

long t = 0;
bool state = true;
void loop() {
  // Make sure device handles messages (i.e. mqtt messages)

  //int val = digitalRead(D1);  // read input value
/*  if (millis() -t > 3000) {
    t = millis();
    state = !state;
    device.changeState(state);
  }
*/
  delay(100);
}