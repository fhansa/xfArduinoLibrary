#ifndef XFMQTT_H
#define XFMQTT_H
/*
 *  xfMQTT.h
 * 
 *  Wrapper for PubSubClient-library
 *  Purpose to make MQTT simple to use with separate callbacks for each subscribe.
 *  
 *  Initiation:
 *      
 * 
 *  USAGE:
 *    Create MQTT and initialize :
 *      xfMQTT mqtt();
 *      mqtt.setup(mqtt-server, port, wifi_client, clientId)
 *    
 *    Subscribe:
 *      void mySubscriptionCallback(char *payload) {
 *          Serial.println(payload);
 *      }
 *      
 *      mqtt.subscribe("some/Topic", mySubscriptionCallback); 
 *      ....
 *      // Call loop to poll mqtt queue
 *      mqtt.loop() 
 * 
 *    Publish:
 *      mqtt.publish("some/Topic", "payload", retainedFlag);
 
 */
#include "xfHelper.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define MAX_SUBSCRIBERS 25
typedef void (*t_mqttCallback)(char *payload);

struct t_subscriber {
    char *topic;
    t_mqttCallback callback;
};

class xfMQTT {
    protected:
        // List of subscriptions with topic and callbacks
        t_subscriber subscribers[MAX_SUBSCRIBERS];
        uint8_t numberOfSubscribers;

        PubSubClient m_mqtt;
        char m_clientId[20];
        String m_lastWill;
        String m_lastWillPayload;

        void mqttCallback(char* topic, byte* payload, unsigned int length);
    public:
        void setup(const char *host, int port, WiFiClient& wifi, bool autoconnect, const char* clientId = NULL, 
                    char *lastWill = NULL, char *lastWillPayload = "offline");
        
        // MQTT Subscription to topic
        bool subscribe(const char *topic, t_mqttCallback callback);
        
        // MQTT publish
        bool publish(const char *topic, const char *payload, bool retained = false);
        
        // Connection, state and Loop method
        bool connect();
        int state();
        boolean loop(int delay = 0);
        boolean connected();
};
#endif XFMQTT_H
