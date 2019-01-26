/*
 *  xfWifiManager.h
 * 
 *  Wrapper for WifiManager to add MQTT-configuration
 * 
 *  Usage:
 * 
 *  TODO:
 *    - Make Wifi-reset configurable
 *    - Make Wifi-reset possible to exclude on compile time
 */
#include <ESP8266WiFi.h>
#include <WiFiManager.h> 
#include <ESP8266WebServer.h>
#include "xfConfig.h"

#define  XFMQTTCONFIGFILE "/mqttconfig.json"        // Name of config file to store mqtt-settings
#define  DEFAULT_WIFI_PORT 8990                     // Portnumber for wifi reset commando

class xfWifiManager {
    protected:
        // Access point name and password
        char ap_name[25];
        char ap_pwd[25];

        // Properties for MQTT 
        bool m_configMQTT;
        char *m_mqttServer;
        char *m_mqttPort;
        char *m_mqttUsername;
        char *m_mqttPassword;

        // WifiServer to enable wifi-reset
        WiFiServer *m_wifiServer;
        void parseWebCall(WiFiClient client);
    public:
        // Constructor, 
        //      Take Access Point credentials and flag indicating if mqtt should be configured or not as input
        xfWifiManager(char *apName, char *apPwd, bool configMQTT);

        // Getter and setters for mqtt
        char *mqttServer();
        char *mqttPort();
        char *mqttUsername();
        char *mqttPassword();
        void setMqttServer(const char *server);
        void setMqttPort(const char *port);
        void setMqttUsername(const char *username);
        void setMqttPassword(const char *password);
        void saveMQTTConfiguration();


        // Turn on and off wifi reset
        void allowReset(bool allow);
      
        // Setup WiFi
        bool setupWifi(bool forceAP = false);
        void resetWifi();
        // Loop function - always call this from loop()
        void handle();
};
