#include "xfMQTT.h"

void xfMQTT::mqttCallback(char* topic, byte* payload, unsigned int length) {
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

xfMQTT::xfMQTT(char *host, int port, WiFiClient& wifi) {
    m_mqtt = new PubSubClient(wifi);
    m_mqtt->setServer(host, port);
    String cId = "esp" + String(random(0xffff), HEX);
    strcpy(m_clientId, cId.c_str());
    // Register message callback
    m_mqtt->setCallback([this] (char* topic, byte* bytePayload, unsigned int length) { this->mqttCallback(topic, bytePayload, length); });
    numberOfSubscribers = 0;
}

void xfMQTT::subscribe(const char *topic, t_mqttCallback callback) {

    this->subscribers[numberOfSubscribers].topic = (char *)malloc(strlen(topic) + 1);
    strcpy(this->subscribers[numberOfSubscribers].topic, topic);
    this->subscribers[numberOfSubscribers].callback = callback;
    this->numberOfSubscribers++;

    this->m_mqtt->subscribe(topic);

}

bool xfMQTT::publish(const char *topic, const char *payload, bool retained) {
    return this->m_mqtt->publish(topic, payload, retained);
}

bool xfMQTT::connect(char *clientId) {
    strcpy(m_clientId, clientId);
    return m_mqtt->connect(clientId, "", "");
}

int xfMQTT::state() { return m_mqtt->state(); }
boolean xfMQTT::connected() { return m_mqtt->connected(); }
boolean xfMQTT::loop(int delay) { 
  static unsigned int mqttDelay = 0;
 
  if(millis() - mqttDelay > delay) {
    if(!m_mqtt->connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (m_mqtt->connect(m_clientId)) {
            Serial.println("connected");
            // Once connected, publish an announcement...
        } else {
            Serial.print("failed, rc=");
            Serial.print(m_mqtt->state());
        }
    } else {
      return m_mqtt->loop(); 
    }
  }
}
