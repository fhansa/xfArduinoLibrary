#include "xfMQTT.h"

void xfMQTT::mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.println(topic);
    for(int i=0; i<this->numberOfSubscribers; i++) {
        if (strcmp(topic, this->subscribers[i].topic) == 0) {
            char payloadStr[length + 1];
            memcpy(payloadStr, payload, length);
            payloadStr[length] = '\0';
            this->subscribers[i].callback(payloadStr);
            break;
        }
    }
}

xfMQTT::xfMQTT() {
    //m_mqtt = new PubSubClient(wifi);
    //m_mqtt->setServer(host, port);
    // Register message callback
}

void xfMQTT::setup(const char *host, int port, WiFiClient& wifi, bool autoconnect, const char* clientId) {
    m_mqtt.setClient(wifi);
    m_mqtt.setServer(host, port);
    if (clientId == NULL) {
        String cId = "esp" + String(random(0xffff), HEX);
        strcpy(m_clientId, cId.c_str());
    } else {
        strcpy(m_clientId, clientId);
    }
    m_mqtt.setCallback([this] (char* topic, byte* bytePayload, unsigned int length) { this->mqttCallback(topic, bytePayload, length); });
    if(autoconnect) connect();
    numberOfSubscribers = 0;
}

bool xfMQTT::subscribe(const char *topic, t_mqttCallback callback) {

    this->subscribers[numberOfSubscribers].topic = (char *)malloc(strlen(topic) + 1);
    strcpy(this->subscribers[numberOfSubscribers].topic, topic);
    this->subscribers[numberOfSubscribers].callback = callback;
    this->numberOfSubscribers++;

    return this->m_mqtt.subscribe(topic, 1);

}

bool xfMQTT::publish(const char *topic, const char *payload, bool retained) {
    return this->m_mqtt.publish(topic, payload, retained);
}

bool xfMQTT::connect() {
    return m_mqtt.connect(m_clientId, "", "");
}

int xfMQTT::state() { return m_mqtt.state(); }
boolean xfMQTT::connected() { return m_mqtt.connected(); }
boolean xfMQTT::loop(int delay) { 
  static unsigned int mqttDelay = 0;
 
  if(delay == 0 || millis() - mqttDelay > delay) {
    if(!this->connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (this->connect()) {
            Serial.println("connected");
            // Once connected, publish an announcement...
        } else {
            Serial.print("failed, rc=");
            Serial.print(this->state());
        }
    } else {
      return m_mqtt.loop(); 
    }
  }
}
