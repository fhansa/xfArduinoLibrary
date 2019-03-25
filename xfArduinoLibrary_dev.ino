#include "xfArduinoLibrary.h"
#include "xfMQTTDevice.h"

xfMQTTBinarySensor device("home", "XFTESTER");
xfMQTT mqtt;
WiFiClient wifiClient;
xfWifiManager wifiMgr("xftester", "update", false, false);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("TEST OF xfArduinoLibrary");

  //pinMode(D1, INPUT_PULLUP);

  String t = "";

  wifiMgr.setupWifi();

  // INIT mqtt
  mqtt.setup("192.168.1.228", 1883, wifiClient, true, "XFTEST");
  mqtt.connect();

  device.m_mqtt = &mqtt;  
  device.publishDiscovery();
  device.publishAvailability()
}

long t = 0;
bool state = true;
void loop() {
  // put your main code here, to run repeatedly:
  mqtt.loop();
  //int val = digitalRead(D1);  // read input value
  if (millis() -t > 3000) {
    t = millis();
    state = !state;
    device.changeState(state);
  }

  delay(100);
}