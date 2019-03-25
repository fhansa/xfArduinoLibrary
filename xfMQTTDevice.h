#include <arduino.h>
#include <ArduinoJson.h>

#include "xfMQTT.h"
#include "xfConfig.h"
/*
 *  Discovery:
 *      publish message on MQTT:
 *          <discovery-prefix>/<devicetype>/<name of device>/config
 *          Device type is specified in base class. 
 *          Discovery-prefix and name of device is specified by user
 * 
 * 
 * 
 */

// --------------------------------------------------------------------------
//  Default values for all MQTT-properties
//
#define DEF_AVAILABILITY_TOPIC                  "availability"
#define DEF_UNIQUE_ID_PREFIX                    "xfdevice_"
#define DEF_NAME_PREFIX                         "espdevice_"
#define DEF_DISCOVERY_PREFIX                    "homeassistant"
#define DEF_PAYLOAD_AVAILABILITY_ONLINE         "online"
#define DEF_PAYLOAD_AVAILABILITY_OFFLINE        "offline"

// NOT IMPLEMETED
/*
#define DEF_DEVICE_IDENTIFIERS                  ""
#define DEF_DEVICE_CONNECTIONS                  ""
#define DEF_DEVICE_MANUFACTURER                 ""
#define DEF_DEVICE_MODEL                        ""
#define DEF_DEVICE_NAME                         ""
#define DEF_DEVICE_SW_VERSION                   ""

#define DEF_JSON_ATTRIBUTES                     ""
*/

class xfMQTTDevice {
    protected:

        // Device type is specified in sub class
        virtual String getDeviceType();

    public: 
        // Discovery properties
        String discoveryPrefix;


        // Device properties
        String unique_id;
        String name;

        // Reference to xfMQTT to execute all messaging
        xfMQTT *m_mqtt;

        // Topics
        String baseTopic;
        String availability_topic;
        String availability_online;
        String availability_offline;

        
        // Publish discovery to Home Assistant
        void publishDiscovery();
        virtual void populateJson(JsonObject& json);

        // Availability 
        void publishAvailability();


        xfMQTTDevice(char *baseTopic, char *deviceName, char *discoveryPrefix);
        xfMQTTDevice(char *baseTopic, char *deviceName) : xfMQTTDevice(baseTopic, deviceName, baseTopic) {};
        xfMQTTDevice(char *baseTopic) : xfMQTTDevice(baseTopic, DEF_NAME_PREFIX, baseTopic) {};
        void availabilityOn();
        void availabilityOff();



};


#define DEF_STATE_TOPIC              "state"
#define DEF_PAYLOAD_STATE_ON         "on"
#define DEF_PAYLOAD_STATE_OFF        "off"

class xfMQTTBinarySensor : public xfMQTTDevice {
    public:
        virtual String getDeviceType();
        xfMQTTBinarySensor(char *baseTopic, char *deviceName, char *discoveryPrefix); // : xfMQTTDevice(baseTopic, deviceName, discoveryPrefix) {};
        xfMQTTBinarySensor(char *baseTopic, char *deviceName) : xfMQTTBinarySensor(baseTopic, deviceName, baseTopic) {};
        xfMQTTBinarySensor(char *baseTopic) : xfMQTTBinarySensor(baseTopic, DEF_NAME_PREFIX, baseTopic) {};

        virtual void populateJson(JsonObject& json);

        bool state;
        String stateTopic;
        String payload_state_on;
        String payload_state_off;

        String device_class;


        void changeState(bool state);
    
};


class xfMQTTLight : xfMQTTDevice {
    // State on/off command-topic

    // Brightness 0-255 brightness_topic
    // brightness_scale??

    // color_temp_state_topic

    // hs_command_topic

    // white_value_command_topic

    // xy_command_topic

    // Rgb 0-255, 0-255, 0-255

    // Effect <custom>

    // List of effects

};
