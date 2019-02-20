/* xfArduinoLibrary - xfConfig
 * Copyright Fredrik Santander 2019
 * MIT License
 *
 * This example shows how to use xfConfig with struct serialization.
 * 
 *  The sketch will try to load configuration from filesystem. First time
 *  there will be no file and default values will be printed to serial. 
 *  After reset the new values will be read from filesystem and printed to serial.
 * 
 */
#include <xfConfig.h>

/*
 * Definition of configuration structure
 */
typedef struct config_t {
  String host;
  int port;

  XF_CONFIG(  
    XF_CONFIGITEM(port, 1883)
    XF_CONFIGITEM(host, "mqttserver.com")
  );
  
};

void setup() {
  Serial.begin(115200);
  Serial.println("===== CONFIGTEST =====");
  config_t conf;

  Serial.println("READING CONFIG");
  xfConfig.readConfig(&conf);
  
  Serial.print("host: "); Serial.println(conf.host);
  Serial.print("port: "); Serial.println(conf.port);

  Serial.println("CHANGING CONFIG");
  conf.host = "google.com";
  conf.port = 80;
  xfConfig.saveConfig(conf);
  
  Serial.println("RESETTING");
  delay(2000);
  ESP.reset();

}

void loop() {
  // put your main code here, to run repeatedly:
}