#ifndef XFWIFIMANAGER_H
#define XFWIFIMANAGER_H
/*  xfArduinoLibrary - xfWiFiManager
 *  Copyright Fredrik Santander 2019
 *  MIT License
 * 
 *  Wrapper for WifiManager to add MQTT-configuration
 * 
 *  Usage:
 *      see github
 * 
 *  // TODO: Make Wifi-reset configurable
 *  // TODO: Make Wifi-reset possible to exclude on compile time
 */
#include "xfHelper.h"
#include <ESP8266WiFi.h>
#include <WiFiManager.h> 
#include <ESP8266WebServer.h>
#include "xfConfig.h"

#define  DEFAULT_WIFI_PORT 8990                                 // Portnumber for wifi reset commando

#define MAX_LEN_DHCP    10
#define MAX_LEN_IPADR   20

// Structure for extended configuration
struct extendedConfig_t {

    // IP-settings
    uint8_t dhcp;
    String staticIP;
    String gatewayIP;
    String netmask;

    // MQTT
    String host;
    int port;
    String username;
    String password;

    XF_CONFIG(
        XF_CONFIGITEM(dhcp, 1)
        XF_CONFIGITEM(staticIP, String(""))
        XF_CONFIGITEM(gatewayIP, String(""))
        XF_CONFIGITEM(netmask, "")
        XF_CONFIGITEM(host, "")
        XF_CONFIGITEM(port, 0)
        XF_CONFIGITEM(username, "")
        XF_CONFIGITEM(password, "")
    );
};

class xfWifiManager {
    protected:
        // Access point name and password
        char ap_name[25];
        char ap_pwd[25];

        // WifiServer to enable wifi-reset
        WiFiServer *m_wifiServer;
        void parseWebCall(WiFiClient client);
        bool configMQTT;
    public:
        // Constructor, 
        //      Take Access Point credentials and flag indicating if mqtt should be configured or not as input
        xfWifiManager(char *apName, char *apPwd, bool configMQTT, bool allowStaticIp = true);

        // Configuration structure
        extendedConfig_t configuration;

        void saveConfiguration();
        void readConfiguration();

        // Turn on and off wifi reset
        void allowReset(bool allow);
        bool allowStaticIp;
        
        // Setup WiFi
        bool setupWifi(bool forceAP = false);
        
        // Reset WiFi
        void resetWifi();
        
        // Reset (Clear) Configuration
        void resetConfiguration();

        // Loop function - always call this from loop()
        void handle();
};
#endif // XFWIFIMANAGER_H