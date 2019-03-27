#include "xfWifiManager.h"

#define  XFEXTENDEDCONFIGFILE "./xfextendedconfig.json"         // Name of config file to store mqtt-settings


 bool saveConfig = false;

xfWifiManager::xfWifiManager(char *apName, char *apPwd, bool configMQTT, bool allowStaticIp)
{
    strcpy(this->ap_name, apName);
    strcpy(this->ap_pwd, apPwd);
    this->configMQTT = configMQTT;
    this->allowStaticIp = allowStaticIp;
    saveConfig = false;
}


void xfWifiManager::readConfiguration() {
  xfConfigClass conf(XFEXTENDEDCONFIGFILE);
  conf.readConfig(&configuration);
}

void xfWifiManager::saveConfiguration() {
  xfConfigClass conf(XFEXTENDEDCONFIGFILE);
  conf.saveConfig(configuration);
}

//
// WifiManager - Setup networking
//
bool xfWifiManager::setupWifi(bool forceAP) {
    // Initiate and set save config callback 
    WiFiManager wifiMgr;
    saveConfig = false;
    wifiMgr.setSaveConfigCallback([]() { saveConfig = true; } );

    // get configuration 
    if(configMQTT || allowStaticIp) {
      xfHelper::xfdebug("xfWifiMgr* Reading extended config");
      this->readConfiguration();
    }

    //
    //  Adding extra parameters for the setup screen
    //
    String dhcp;
    dhcp = configuration.dhcp == 1 ? "YES" : "NO";
    WiFiManagerParameter param_dhcp("DHCP", "DHCP", dhcp.c_str() , 10);
    WiFiManagerParameter param_static_ip("staticip", "static ip", configuration.staticIP.c_str(), 20);
    WiFiManagerParameter param_static_gateway("gateway", "static gateway", configuration.gatewayIP.c_str(), 20);
    WiFiManagerParameter param_static_netmask("netmask", "netmask", configuration.netmask.c_str(), 20);

    String port(configuration.port);

    WiFiManagerParameter param_mqtt_server("server", "mqtt server", configuration.host.c_str(), 50);
    WiFiManagerParameter param_mqtt_port("port", "mqtt port", port.c_str(), 6);
    WiFiManagerParameter param_mqtt_username("user", "mqtt username", configuration.username.c_str(), 25);
    WiFiManagerParameter param_mqtt_password("password", "mqtt password", configuration.password.c_str(), 25);
    if(this->configMQTT) {
        wifiMgr.addParameter(&param_mqtt_server);
        wifiMgr.addParameter(&param_mqtt_port);
        wifiMgr.addParameter(&param_mqtt_username);
        wifiMgr.addParameter(&param_mqtt_password);
    }

    if(this->allowStaticIp) {
        wifiMgr.addParameter(&param_dhcp);
        wifiMgr.addParameter(&param_static_ip);
        wifiMgr.addParameter(&param_static_gateway);
        wifiMgr.addParameter(&param_static_netmask);
    }

    // 
    //  Do the autoconnect magic....
    //
    if(forceAP) {
        xfHelper::xfdebug("xfWifiMgr* Forcing AP mode");
        if (!wifiMgr.startConfigPortal(ap_name, ap_pwd)) {
            xfHelper::xfdebug("xfWifiMgr* ERROR: failed to connect and hit timeout");
            delay(3000);
            ESP.reset();
            delay(5000);
        }
    } else {
        xfHelper::xfdebug("xfWifiMgr* Connect...");        
        if(this->allowStaticIp && configuration.dhcp != 1) {
            xfHelper::xfdebug("xfWifiMgr* Using static IP");
            IPAddress _ip, _gw, _sn;
            _ip.fromString(configuration.staticIP);
            _gw.fromString(configuration.gatewayIP);
            _sn.fromString(configuration.netmask);
            wifiMgr.setSTAStaticIPConfig(_ip, _gw, _sn);
        } 
        if(!wifiMgr.autoConnect(ap_name, ap_pwd)) {
            xfHelper::xfdebug("xfWifiMgr* ERROR: failed to connect and hit timeout");
            delay(3000);
            ESP.reset();      //reset and try again, or maybe put it to deep sleep
            delay(5000);
        }
    }
  
  // Save configuration - if needed
  if(saveConfig) {
      if(this->configMQTT) {  
        configuration.host = param_mqtt_server.getValue();
        configuration.port = atoi(param_mqtt_port.getValue());
        configuration.username = param_mqtt_username.getValue();
        configuration.password = param_mqtt_password.getValue(); 
      }

      if(this->allowStaticIp) {
        configuration.dhcp = strcmp(param_dhcp.getValue(), "YES") == 0 ? 1 : 0;
        configuration.staticIP = param_static_ip.getValue();
        configuration.gatewayIP = param_static_gateway.getValue();
        configuration.netmask = param_static_netmask.getValue();
      }

      saveConfiguration();

    }


    Serial.println("local ip");
    Serial.println(WiFi.localIP());
}

//
// Handle WebServer-call
//
void xfWifiManager::handle() {
    // Check webserver interface calls
  WiFiClient client = m_wifiServer->available();
  if(client) {
    this->parseWebCall(client);
  }
}

void xfWifiManager::parseWebCall(WiFiClient client) {
  bool resetWifi = false;
  String header("");
  String currentLine = "";   
  while (client.connected()) {              // loop while the client's connected
     if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK\n");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println("OK"); 
            Serial.println(header);
            Serial.println(currentLine);
  
            if(header.indexOf("GET /reset") >= 0) {
              resetWifi = true;
            }
            
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
     }
  }
  // Close the connection
  client.stop();

  if(resetWifi) {
    setupWifi(true);
  }
}

void xfWifiManager::allowReset(bool allow) {
  if (allow) {
    // Setup webserver
      xfHelper::xfdebug("xfWifiMgr* Allowing reset on port:");
      Serial.println(DEFAULT_WIFI_PORT);
      m_wifiServer = new WiFiServer(DEFAULT_WIFI_PORT);
      m_wifiServer->begin();
  } else {
    if(m_wifiServer)
      free(m_wifiServer);
     m_wifiServer = NULL;
  }
}
