# xfArduinoLibrary
Collection of wrappers for some Arduino libraries. Purpose to make the reuse simpler

## xfConfig
Library to save and retrieve configuration for ESP8266. The configuration is stored as a file in ESP8266 filesystem and can be read/saved using json or by using struct serialization. 

Library has a global instance for ./config.js. xfConfig

To not use the global instance: use xfConfigClass(filename)

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