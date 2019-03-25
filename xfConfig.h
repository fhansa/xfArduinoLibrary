/*  xfArduinoLibrary - xfConfig
 *  Copyright Fredrik Santander 2019
 *  MIT License
 * 
 *      Configuration manager for ESP8266. 
 *      Library makes it possible to store and retrieve configuration. 
 *      Configuration is stored as json-file in ESP-filesystem. 
 *      Configuration can be serialized to and from a c++ struct using macros.
 * 
 *      Library contains a global object named xfConfig to access an instance 
 *      using the default filename (config.js).
 * 
 *  TODO:
 *      - optimize memory allocation for ArduinoJson-buffer and filename 
 *      - enable logging to other than serialized
 *      - option to turn off logging
 *      - better error handling (i.e. return bool from read/save)
 * 
 */
#ifndef XFCONFIG_H
#define XFCONFIG_H
#include <FS.h>
#include <ArduinoJson.h>  

/*
 *  Forward declarations
 */
class xfConfigClass;
extern xfConfigClass xfConfig;
extern StaticJsonBuffer<512> jsonBuffer;

/*
 *      Macros for serializing json <-> struct
 * 
 */
#define XF_CONFIG(...) \
  void handleJson(JsonObject &json, int mode) { \
    __VA_ARGS__ \
  }; \
  void parseJson(JsonObject &json) { \
    handleJson(json, 1); \
  }; \
  JsonObject &getJson() { \
    JsonObject& json = jsonBuffer.createObject(); \
    handleJson(json, 0); \
    return json; \  
  }; 

#define XF_CONFIGITEM(member, defaultValue) \
  if (mode == 1) { member = json[#member].success() ? json[#member].as<decltype(member)>() : defaultValue; } else { json[#member] = member; } 

#define XFCONFIG_DEFAULT_FILENAME "/config.json"

/*
 *  xfConfigClass
 */
class xfConfigClass {
    protected:
        // Error object
        JsonObject &errorJson();    

        // Filename of configuration file (default './config.js')
        char m_filename[50]; 
    public:
        // Always construct using filename
        xfConfigClass(char *filename);

        // read and save using json-object
        JsonObject &readConfigAsJson();
        void saveConfigFromJson(JsonObject &json);
        
        // read and save using struct 
        template <typename T>
        void readConfig(T *configStructure);
        template <typename T>
        void saveConfig(T configStructure);

        // Delete config file
        void deleteConfig();

        // Delete all files on filesystem 
        void purgeFS(); 

};

/*
 *      Implementation of template methods
 */
template <typename T>
void xfConfigClass::readConfig(T *configStructure) {
  JsonObject &json = this->readConfigAsJson();
  configStructure->parseJson(json);
}

template <typename T>
void xfConfigClass::saveConfig(T configStructure) {
  JsonObject &json = configStructure.getJson();
  this->saveConfigFromJson(json);
}

#endif
