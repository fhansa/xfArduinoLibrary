
#include "xfWifiManager.h"



 bool saveConfig = false;

xfWifiManager::xfWifiManager(char *apName, char *apPwd, bool configMQTT, bool allowStaticIp)
: m_mqttServer(NULL),
  m_mqttPort(NULL),
  m_mqttUsername(NULL),
  m_mqttPassword(NULL),
  m_wifiServer(NULL)
{
    strcpy(this->ap_name, apName);
    strcpy(this->ap_pwd, apPwd);
    this->m_configMQTT = configMQTT;
    this->allowStaticIp = allowStaticIp;
    saveConfig = false;
}


void xfWifiManager::setDHCP(const char *dhcp) {
    strlcpy(m_dhcp, dhcp, MAX_LEN_DHCP);
}
void xfWifiManager::setStaticIp(const char *staticIp) {
    strlcpy(m_staticIp, staticIp, MAX_LEN_IPADR);
}
void xfWifiManager::setStaticGateway(const char *staticGateway) {
    strlcpy(m_staticGateway, staticGateway, MAX_LEN_IPADR);
}

void xfWifiManager::setMqttServer(const char *server) {
    if (m_mqttServer)  {
        free(m_mqttServer);
    }
    m_mqttServer = (char *)malloc(sizeof(server) + sizeof(char));
    strcpy(m_mqttServer, server);
}


void xfWifiManager::setMqttPort(const char *port) {
    if (m_mqttPort)  {
        free(m_mqttPort);
    }
    m_mqttPort = (char *)malloc(sizeof(port) + sizeof(char));
    strcpy(m_mqttPort, port);
}

void xfWifiManager::setMqttUsername(const char *username) {
    if (m_mqttUsername)  {
        free(m_mqttUsername);
    }
    m_mqttUsername = (char *)malloc(sizeof(username) + sizeof(char));
    strcpy(m_mqttUsername, username);
}

void xfWifiManager::setMqttPassword(const char *password) {
    if (m_mqttPassword)  {
        free(m_mqttPassword);
    }
    m_mqttPassword = (char *)malloc(sizeof(password) + sizeof(char));
    strcpy(m_mqttPassword, password);
}

char *xfWifiManager::mqttPort() {
    return m_mqttPort;
}

char *xfWifiManager::mqttServer() {
    return m_mqttServer;
}

char *xfWifiManager::mqttUsername() {
    return m_mqttUsername;
}

char *xfWifiManager::mqttPassword() {
    return m_mqttPassword;
}

void xfWifiManager::readConfiguration() {
    xfConfigClass config(XFMQTTCONFIGFILE);
    JsonObject &j = config.readConfigAsJson();

    setMqttServer(j["mqttServer"] | " ");
    setMqttPort(j["mqttPort"] | " ");
    setMqttUsername(j["mqttUsername"] | " ");
    setMqttPassword(j["mqttPassword"] | " ");  

    setDHCP(j["DHCP"] | "Y");
    setStaticIp(j["staticIp"] | "");
    setStaticGateway(j["staticGateway"] | "");

    Serial.print("Server"); Serial.println(m_mqttServer);
    Serial.print("Port"); Serial.println(m_mqttPort);
    Serial.print("User"); Serial.println(m_mqttUsername);
    Serial.print("Pwd"); Serial.println(m_mqttPassword);
}

void xfWifiManager::saveConfiguration() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqttServer"] = m_mqttServer;
    json["mqttPort"] = m_mqttPort;
    json["mqttUsername"] = m_mqttUsername;
    json["mqttPassword"] =  m_mqttPassword;
    json["DHCP"] = m_dhcp;
    json["staticIp"] = m_staticIp;
    json["staticGateway"] = m_staticGateway;

    xfConfigClass config(XFMQTTCONFIGFILE);
    config.saveConfigFromJson(json);
}

//
// WifiManager - Setup networking
//
bool xfWifiManager::setupWifi(bool forceAP) {
    // Initiate and set save config callback 
    WiFiManager wifiMgr;
    saveConfig = false;
    wifiMgr.setSaveConfigCallback([]() { saveConfig = true; } );

    this->readConfiguration();

    //
    //  Adding extra parameters for the setup screen
    //
    WiFiManagerParameter param_dhcp("DHCP", "DHCP", m_dhcp, 10);
    WiFiManagerParameter param_static_ip("staticip", "static ip", m_staticIp, 20);
    WiFiManagerParameter param_static_gateway("gateway", "static gateway", m_staticGateway, 20);

    WiFiManagerParameter param_mqtt_server("server", "mqtt server", m_mqttServer, 50);
    WiFiManagerParameter param_mqtt_port("port", "mqtt port", m_mqttPort, 6);
    WiFiManagerParameter param_mqtt_username("user", "mqtt username", m_mqttUsername, 25);
    WiFiManagerParameter param_mqtt_password("password", "mqtt password", m_mqttPassword, 25);
    if(this->m_configMQTT) {
        wifiMgr.addParameter(&param_mqtt_server);
        wifiMgr.addParameter(&param_mqtt_port);
        wifiMgr.addParameter(&param_mqtt_username);
        wifiMgr.addParameter(&param_mqtt_password);
    }

    if(this->allowStaticIp) {
        wifiMgr.addParameter(&param_dhcp);
        wifiMgr.addParameter(&param_static_ip);
        wifiMgr.addParameter(&param_static_gateway);
    }

    // 
    //  Do the autoconnect magic....
    //
    if(forceAP) {
        Serial.println("Entering setup mode");
        if (!wifiMgr.startConfigPortal(ap_name, ap_pwd)) {
            Serial.println("failed to connect and hit timeout");
            delay(3000);
            ESP.reset();
            delay(5000);
        }
    } else {
        Serial.println(m_dhcp);
        if(this->allowStaticIp && strcmp(m_dhcp, "N") == 0) {
            Serial.println("Using static IP");
            IPAddress _ip, _gw, _sn;
            _ip.fromString(m_staticIp);
            _gw.fromString(m_staticGateway);
            _sn.fromString("255.255.255.0");
            wifiMgr.setSTAStaticIPConfig(_ip, _gw, _sn);
        }

        if(!wifiMgr.autoConnect(ap_name, ap_pwd)) {
            Serial.println("failed to connect and hit timeout");
            delay(3000);
            ESP.reset();      //reset and try again, or maybe put it to deep sleep
            delay(5000);
        }
    }
  
  // Save configuration - if needed
  if(saveConfig) {
      if(this->m_configMQTT) {

        setMqttServer(param_mqtt_server.getValue());
        setMqttPort(param_mqtt_port.getValue());
        setMqttUsername(param_mqtt_username.getValue());
        setMqttPassword(param_mqtt_password.getValue()); 
      }

      if(this->allowStaticIp) {
        setDHCP(param_dhcp.getValue());
        setStaticIp(param_static_ip.getValue());
        setStaticGateway(param_static_gateway.getValue());
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
      Serial.print("Allowing reset on port:");
      Serial.println(DEFAULT_WIFI_PORT);
      m_wifiServer = new WiFiServer(DEFAULT_WIFI_PORT);
      m_wifiServer->begin();
  } else {
    if(m_wifiServer)
      free(m_wifiServer);
     m_wifiServer = NULL;
  }
}
