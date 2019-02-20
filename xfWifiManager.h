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

#define MAX_LEN_DHCP    10
#define MAX_LEN_IPADR   20


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
        char m_dhcp[MAX_LEN_DHCP];
        char m_staticIp[MAX_LEN_IPADR];
        char m_staticGateway[MAX_LEN_IPADR];

        // WifiServer to enable wifi-reset
        WiFiServer *m_wifiServer;
        void parseWebCall(WiFiClient client);
    public:
        // Constructor, 
        //      Take Access Point credentials and flag indicating if mqtt should be configured or not as input
        xfWifiManager(char *apName, char *apPwd, bool configMQTT, bool allowStaticIp = true);

        // Getter and setters for mqtt
        char *mqttServer();
        char *mqttPort();
        char *mqttUsername();
        char *mqttPassword();
        void setMqttServer(const char *server);
        void setMqttPort(const char *port);
        void setMqttUsername(const char *username);
        void setMqttPassword(const char *password);

        void setDHCP(const char *dhcp);
        void setStaticIp(const char *staticIp);
        void setStaticGateway(const char *staticGateway);

        void saveConfiguration();
        void readConfiguration();

        // Turn on and off wifi reset
        void allowReset(bool allow);
        bool allowStaticIp;
        // Setup WiFi
        bool setupWifi(bool forceAP = false);
        void resetWifi();
        // Loop function - always call this from loop()
        void handle();
};
