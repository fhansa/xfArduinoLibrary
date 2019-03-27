#ifndef XFMQTTDEVICE_H
#define XFMQTTDEVICE_H
/*
 *  xfMQTTDevice
 *      Implementation of wrapper for Home Assistant-style 
 *      MQTT devices. The wrapper will implement common functionality 
 *      for a mqtt device and let user/developer create their own devices 
 *      with a minimum of coding. 
 * 
 *      Documentation of MQTT device: 
 *          https://www.home-assistant.io/docs/mqtt/discovery/
 * 
 *  Content:
 *    xfMQTTDevice        
 *      - Base class for a MQTT-device
 *    xfMQTTBinarySensor  
 *      - Implementation of Binary sensor
 *        https://www.home-assistant.io/components/binary_sensor.mqtt/
 *    xfMQTTLight         
 *      - Implementation of MQTT light 
 *        https://www.home-assistant.io/components/light.mqtt/
 * 
 * 
 *  Usage: 
 *      Implementation of your device is done by utilizing corresponding class. 
 *      Each class exposes the neccessary properties, callbacks and methods to be able to 
 *      completely implement a mqtt device. 
 *      
 *      Some information is required for all devices while other is based on type of device.  
 *      
 *  Discovery:
 *      publish message on MQTT:
 *          <discovery-prefix>/<devicetype>/<name of device>/config
 *          Device type is specified in base class. 
 *          Discovery-prefix and name of device is specified by user
 *  NOT IMPLEMETED
 *      device portion of the discovery payload
 *      json-attributes in discovery payload is not implemented
 * 
 *  TODOS
 * 
 *  // TODO: Create examples and tests for xfMQTTDevices
 *  // TODO: Better setup of device to hide setupMQTT if xfWifiManager is used
 *  // IDEA: Make everything more configurable (e.g. topics, connection etc...)
 * 
 */


#include <arduino.h>
#include <ArduinoJson.h>
#include "xfMQTT.h"
#include "xfConfig.h"

// --------------------------------------------------------------------------
//  Default values for MQTT-properties
//
#define DEF_UNIQUE_ID_PREFIX                    "xfdevice_"
#define DEF_NAME_PREFIX                         "espdevice_"
#define DEF_DISCOVERY_PREFIX                    "homeassistant"

// --------------------------------------------------------------------------
//  Base MQTT device - this instance cannot be directly used. Always use 
//                     a specific sub class.
// 
class xfMQTTDevice {
    protected:
        // Device type must be specified in sub class
        // Corresponds to Home Assistant device type 
        virtual String getDeviceType();

        // Reference to xfMQTT to execute all messaging
        xfMQTT *m_mqtt;

    public: 
        // MQTT 
        void setupMQTT(char *host, int port, char *username, char *password);
        void messageLoop();
        String mqttBaseTopic;

        // Discovery properties
        String discoveryPrefix;
        virtual void getDiscoveryPayload(JsonObject& json); // Always call parent when overridden!!!
        void publishDiscovery();

        // Device properties
        String unique_id;
        String name;

        // Availability 
        void publishAvailability();

        // Construction Time again
        xfMQTTDevice(char *baseTopic, char *deviceName, char *discoveryPrefix);
        xfMQTTDevice(char *baseTopic, char *deviceName) :
            xfMQTTDevice(baseTopic, deviceName, baseTopic) {};
        xfMQTTDevice(char *baseTopic) :
            xfMQTTDevice(baseTopic, DEF_NAME_PREFIX, baseTopic) {};

        // Actions methods - triggered from client
        void availabilityOn();
        void availabilityOff();

};


// --------------------------------------------------------------------------
//  Binary Sensor
//      see https://www.home-assistant.io/components/binary_sensor.mqtt/
//
//      Simple sensor that reports a binary state (i.e. on/off, open/close, active/inactive)
// 
class xfMQTTBinarySensor : public xfMQTTDevice {
    public:
        // Specify the device type
        virtual String getDeviceType();

        // Create discovery payload
        virtual void getDiscoveryPayload(JsonObject& json);

        // Construction Time again
        xfMQTTBinarySensor(char *baseTopic, char *deviceName, char *discoveryPrefix); 
        xfMQTTBinarySensor(char *baseTopic, char *deviceName) : 
            xfMQTTBinarySensor(baseTopic, deviceName, baseTopic) {};
        xfMQTTBinarySensor(char *baseTopic) : 
            xfMQTTBinarySensor(baseTopic, DEF_NAME_PREFIX, baseTopic) {};

        // *** Binary Sensor specific properties ***
        // State 
        bool state;
        void changeState(bool state); 

        // Device class (type of binary sensor https://www.home-assistant.io/components/binary_sensor/)
        String device_class;


};

// --------------------------------------------------------------------------
//  Light
//      see https://www.home-assistant.io/components/light.mqtt/
//
//      Complete Light device. Extends a binary sensor
// 
class xfMQTTLight : xfMQTTBinarySensor {
    public:
        // Specify the device type
        virtual String getDeviceType();

        // Construction Time again
        xfMQTTLight(char *baseTopic, char *deviceName, char *discoveryPrefix); 
        xfMQTTLight(char *baseTopic, char *deviceName) : 
            xfMQTTLight(baseTopic, deviceName, baseTopic) {};
        xfMQTTLight(char *baseTopic) : 
            xfMQTTLight(baseTopic, DEF_NAME_PREFIX, baseTopic) {};

        // Create discovery payload
        virtual void getDiscoveryPayload(JsonObject& json);

        // *** Light properties ***

        // TODO: Add actions for Light

        // command topic
        String commandTopic;

        // Brightness
        bool enableBrightness;
        int brightness_scale;
        
        // Color temp
        bool enableColorTemp;
        
        // Hue Saturation
        bool enableHueSaturation;

        // White Value
        bool enableWhiteValue;
        int whiteValueScale;

        // XY Color
        bool enableXY;

        // RGB Color
        bool enableRGB;

        // Effect <custom>
        bool enableEffects;
        String effectList;

};

#endif // XFMQTTDEVICE_H