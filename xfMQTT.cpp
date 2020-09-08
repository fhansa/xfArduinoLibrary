/*  xfArduinoLibrary 
 *  Copyright Fredrik Santander 2019
 *  https://github.com/fhansa/xfArduinoLibrary
 *  MIT License
 */
#include "xfMQTT.h"

// Generic callback 
void xfMQTT::mqttCallback(char* topic, byte* payload, unsigned int length) {
    
    // Iterate all subscriber to see who should get an event
    for(int i=0; i<this->numberOfSubscribers; i++) {
        if (strcmp(topic, this->subscribers[i].topic) == 0) {
    
            // Convert payload into a char * 
            char payloadStr[length + 1];
            memcpy(payloadStr, payload, length);
            payloadStr[length] = '\0';
            xfHelper::xfdebug("xfMQTT* Message: %s %s", topic, payloadStr);
            
            // Fire the event
            this->subscribers[i].callback(payloadStr);
            break;
        }
    }
}

// MQTT Setup
void xfMQTT::setup(const char *host, int port, WiFiClient& wifi, bool autoconnect, const char* clientId, char *lastWill, char *lastWillPayload) {
    xfHelper::xfdebug("xfMQTT* Setting up MQTT");
    m_mqtt.setClient(wifi);
    xfHelper::xfdebug("xfMQTT* Host: %s, PPPP: %d",host, port);
    m_mqtt.setServer(host, port);
    if (clientId == NULL) {
        // If client Id is not supplied then create a random ID
        String cId = "esp" + String(random(0xffff), HEX);
        strcpy(m_clientId, cId.c_str());
    } else {
        strcpy(m_clientId, clientId);
    }
    // Save the lastWill (need to pass those on connect later on)
    m_lastWill = String(lastWill);
    m_lastWillPayload = String(lastWillPayload);

    // Register internal callback 
    m_mqtt.setCallback([this] (char* topic, byte* bytePayload, unsigned int length) { this->mqttCallback(topic, bytePayload, length); });
    
    // ... and connect immediately if autoconnect is enabled
    if(autoconnect) {
        xfHelper::xfdebug("xfMQTT* Autoconnecting during setup");
        connect();
    }
    numberOfSubscribers = 0;
}

// Subscribe to topic
bool xfMQTT::subscribe(const char *topic, t_mqttCallback callback) {

    // Register the subscriber (topic and callback)
    this->subscribers[numberOfSubscribers].topic = (char *)malloc(strlen(topic) + 1);
    strcpy(this->subscribers[numberOfSubscribers].topic, topic);
    this->subscribers[numberOfSubscribers].callback = callback;
    this->numberOfSubscribers++;

    // Do the actual subscription
    return this->m_mqtt.subscribe(topic, 1);
}

// Publishing
bool xfMQTT::publish(const char *topic, const char *payload, bool retained) {
    // Simply forward publish message to mqtt-client and report back if anything went wrong
    bool res = this->m_mqtt.publish(topic, payload, retained);
    if(!res) {
        xfHelper::xfdebug("xfMQTT* MQTT Publish Error: &d", m_mqtt.state());
    }
    return res;
}

// Connection logic
bool xfMQTT::connect() {
    bool res = true;

    // Connect with or without lastWill
    if (m_lastWill != NULL) {
        xfHelper::xfdebug("xfMQTT* Connect with lastWill %s:%s", m_lastWill.c_str(), m_lastWillPayload.c_str());
        res = m_mqtt.connect(m_clientId, "", "", m_lastWill.c_str(), 0, 1, m_lastWillPayload.c_str());
    }
    else
        res = m_mqtt.connect(m_clientId, "", "");

    // Do some debug reporting of the outcome
    if (!res)
        xfHelper::xfdebug("xfMQTT* Connect - MQTT Error: %d", m_mqtt.state());
    else 
        xfHelper::xfdebug("xfMQTT* Connect - MQTT Connection successful");

    return res;
}

// Forward methods for state and connected
int xfMQTT::state() { return m_mqtt.state(); }
boolean xfMQTT::connected() { return m_mqtt.connected(); }

// MQTT-loop with reconnection logic
boolean xfMQTT::loop(int delay) { 
    static unsigned int mqttDelay = 0;

    // Delay makes it possible to prevent too frequent calls to mqtt.loop().
    // Delay defaults to 0 but can be provided to avoid delay-logic in main loop
    if(delay == 0 || millis() - mqttDelay > delay) {
        
        if(!this->connected()) {
            // Try to reconnect if not connected 
            // ... but first some nice dbugging info
            xfHelper::xfdebug("xfMQTT* Attempting MQTT connection...");
            
            // And then attempt to connect (any logging is done within connect())
            this->connect();
        } else {
        // Everything is fine, just poll mqtt-queue
        return m_mqtt.loop(); 
        }
    }
}
