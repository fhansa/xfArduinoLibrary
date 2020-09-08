# xfArduinoLibrary

[![MIT Licence](https://badges.frapsoft.com/os/mit/mit.svg?v=103)](https://opensource.org/licenses/mit-license.php)

Collection of wrappers for some Arduino libraries. Purpose to make the reuse simpler

Library contains:

| Library   | Description  |   
|-----------|--------------|
|xfConfig   | Persistant configuration using ESP8266 filesystem with serialization to and from c++ struct 
| xfMQTT    | Wrapper for PubSubClient to make the code easier and more readable. 
| xfOTA | Wrapper for ArduinoOTA
| xfWifiManager | Wrapper for WiFiManager with option to get parameters for MQTT in setup screen. Depends on xfConfig.
| xfMQTTDevice | Implementation of MQTT device support (home assistant style). Contains implementation of various HomeAssistant device; Light, Binary Sensor




## xfConfig
Library to save and retrieve configuration for ESP8266. The configuration is stored as a file in ESP8266 filesystem and can be read/saved using json or by using struct serialization. 

Library has a global instance for ./config.js. xfConfig

To not use the global instance: use xfConfigClass(filename)

### Dependencies
* FS from ESP8266 Core - https://github.com/esp8266/Arduino
* ArduinoJson - https://arduinojson.org

### Usage - struct serialization

```
// Define configuration structure and enable serialization
// by using XF_CONFIG-macros

struct config_t {
    // Configuration properties
    String someText;
    int someNumber;

    // Macro to enable serialization 
    XF_CONFIG(
        XF_CONFIGITEM(someText, "default text")
        XF_CONFIGITEM(someNumber, 0)
    )
};

config_t conf;
xfConfig.readConfig(&conf);

conf.someText = "NewText";
conf.someNumber = 10;

xfConfig.saveConfig(conf);

```
### Usage - json serialization
```

JsonObject &json = xfConfig.readConfigAsJson();

/*
 * To save configuration it is neccessary to create JsonObject 
 * Create buffer for ArduinoJson (can also use DynamicJsonBuffer)
 * StaticJsonBuffer<512> jsonBuffer;
 * JsonObject &json = jsonBuffer.createObject();
 */

json["someText"] = "NewText";
json["someNumber"] = 10;

xfConfig.saveConfigFromJson(json);

```


## xfOTA
Wrapper for ArduinoOTA. Purpose is to have two oneliners to enable OTA. 
For more documentation see ArduinoOTA.

### Dependencies

* ArduinoOTA from ESP8266 Core - https://github.com/esp8266/Arduino

### Usage
```
    // Create instance 
    xfOTA ota;

    // Setup OTA with OTA host name and password
    ota.setup("ESP8266", "otapassword");

    // In loop() 
    ota.loop();

```

## xfMQTTDevice 
Base class for Home Assistant MQTT-devices

## xfMQTTBinarySensor
Implementation of home assistant binary_sensor. 

```
    // Create device 
    char *base = "home";            // base used for all topics 
    char *name = "mysensor";        // name of device
    char *disc = "homeassistant";   // discovery prefix 
    xfMQTTBinarySensor device = xfBinarySensor(base, name, disc);

    // Set MQTT-properties
    char *host = "192.168.1.?";     // IP of MQTT-broker
    int port = 1883;                // port of MQTT-broker
    char *usr = "someone";          // User for MQTT-broker
    char *pwd = "verysecret"        // pwd for MQTT-broker
    device.setupMQTT(hosty, port, usr, pwd);
```
After setup following MQTT retained messages have been sent to inform home assistant about the device characteristics and to say that device is available. 
For details of payload see https://www.home-assistant.io/docs/mqtt/discovery/ and https://www.home-assistant.io/components/binary_sensor.mqtt/ 
```
    // Configuration message
    topic: home/binary_sensor/mysensor/config
    payload: {
        "uniq_id":"mysensor1002",
        "name":"mysensor",
        "~":"home/mysensor",
        "avty_t":"~/availability",
        "pl_avail":"online",
        "pl_not_avail":"online",
        "stat_t":"~/state",
        "pl_on":"on",
        "pl_off":"off"
    }

    // Availabilty message
    topic: home/mySensor/availability
    payload: online
```

Whenever the device wants to change the state (e.g. motion detected, door opened, switch turned on, ...)
```
    // Trigger change of state 
    device.changeState(true);       // Set state to on
```
Results in following MQTT-message
```
    topic: home/mySensor/state
    payload: on
```
