
#include "xfWifiManager.h"



 bool saveConfig = false;

xfWifiManager::xfWifiManager(char *apName, char *apPwd, bool configMQTT)
: m_mqttServer(NULL),
  m_mqttPort(NULL),
  m_mqttUsername(NULL),
  m_mqttPassword(NULL),
  m_wifiServer(NULL)
{
    strcpy(this->ap_name, apName);
    strcpy(this->ap_pwd, apPwd);
    this->m_configMQTT = configMQTT;
    saveConfig = false;
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

//
// WifiManager - Setup networking
//
bool xfWifiManager::setupWifi(bool forceAP) {
    // Initiate and set save config callback 
    WiFiManager wifiMgr;
    saveConfig = false;
    wifiMgr.setSaveConfigCallback([]() { saveConfig = true; } );

    xfConfigClass config(XFMQTTCONFIGFILE);
    JsonObject &j = config.readConfig();

    j.printTo(Serial);
    
    setMqttServer(j["mqttServer"] | "");
    setMqttPort(j["mqttPort"] | "");
    setMqttUsername(j["mqttUsername"] | "");
    setMqttPassword(j["mqttPassword"] | "");    
    //
    //  Adding extra parameters for the setup screen
    //
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
    Serial.println("Use");


    // 
    //  Do the autoconnect magic....
    //
    if(forceAP) {
        if (!wifiMgr.startConfigPortal(ap_name, ap_pwd)) {
            Serial.println("failed to connect and hit timeout");
            delay(3000);
            ESP.reset();
            delay(5000);
        }
    } else {
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

        saveMQTTConfiguration();

      }
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
}



void xfWifiManager::saveMQTTConfiguration() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqttServer"] = m_mqttServer;
    json["mqttPort"] = m_mqttPort;
    json["mqttUsername"] = m_mqttUsername;
    json["mqttPassword"] =  m_mqttPassword;

    xfConfigClass config(XFMQTTCONFIGFILE);
    config.saveConfig(json);
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
