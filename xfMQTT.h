#ifndef XFMQTT_H
#define XFMQTT_H
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
        t_subscriber subscribers[MAX_SUBSCRIBERS];
        uint8_t numberOfSubscribers;
        PubSubClient *m_mqtt;
        char m_clientId[20];
        void mqttCallback(char* topic, byte* payload, unsigned int length);
    public:
        xfMQTT(char *host, int port, WiFiClient& wifi);
        void subscribe(const char *topic, t_mqttCallback callback);
        bool publish(const char *topic, const char *payload, bool retained = false);
        bool connect(char *clientId);
        int state();
        boolean loop(int delay = 0);
        boolean connected();
};
#endif
