/* xfArduinoLibrary - xfConfig
 * Copyright Fredrik Santander 2019
 * MIT License
 *
 * This example shows how to use xfWifiManager 
 *  
 *  WifiManager will try to connect to last known Wifi. 
 *  When fails a AP with name ESPSETUP is created. 
 *  Log on to ESPSETUP and browse to 192.168.1.4 to configure Wifi. 
 * 
 *  Show extended parameters for static IP and MQTT
 */

#include <xfWifiManager.h>


xfWifiManager mgr("ESPSETUP", "verysecret", true, true);

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  mgr.setupWifi(false);
  mgr.allowReset(true);

  Serial.println("=== MQTT Configuration ===");
  Serial.println(mgr.configuration.host);
  Serial.println(mgr.configuration.port);
  Serial.println(mgr.configuration.username);
  Serial.println(mgr.configuration.password);

  Serial.println("=== Network Configuration ===");
  Serial.print("DHCP:"); Serial.println(mgr.configuration.dhcp ? "YES" : "NO");
  Serial.println(mgr.configuration.staticIP);
  Serial.println(mgr.configuration.gatewayIP);
  Serial.println(mgr.configuration.netmask);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  mgr.handle();

}