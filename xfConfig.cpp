/*  xfArduinoLibrary 
 *  Copyright Fredrik Santander 2019
 *  https://github.com/fhansa/xfArduinoLibrary
 *  MIT License
 */
#include "xfConfig.h"

// Global instance 
xfConfigClass xfConfig(XFCONFIG_DEFAULT_FILENAME);
StaticJsonBuffer<512> jsonBuffer;

// 
//  Create error json object
// 
JsonObject &xfConfigClass::errorJson() {
    //DynamicJsonBuffer jsonBuffer;
    JsonObject &error = jsonBuffer.createObject();
    error["msg"] = "Error";
    return error;
}

//
// Constructor 
//
xfConfigClass::xfConfigClass(char *filename) {
  strlcpy(m_filename, filename, 49);
}

//
//  Read Config.json (local file) into variables 
//
JsonObject &xfConfigClass::readConfigAsJson() {

  xfHelper::xfdebug("xfConfig* Reading configuration");
  if(!SPIFFS.begin()) {
    xfHelper::xfdebug("xfConfig* ERROR - Cannot mount FS.");
    return errorJson();
  }

  File configFile = SPIFFS.open(m_filename, "r");
  if(!configFile) {
    xfHelper::xfdebug("xfConfig* ERROR - Cannot open %s", m_filename);
    return errorJson();
  }

  // Read config.json into memory 
  size_t fileSize = configFile.size();
  std::unique_ptr<char []> buf(new char[fileSize]);
  configFile.readBytes(buf.get(), fileSize);

  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if(!json.success()) {
    xfHelper::xfdebug("xfConfig* ERROR - Cannot parse json");
    return errorJson();
  }
  json.printTo(Serial);
  xfHelper::xfdebug("xfConfig* EOF Read Config");

  configFile.close()    ;

  return json;
}

//
//  Save config 
//
void xfConfigClass::saveConfigFromJson(JsonObject &json) {
    xfHelper::xfdebug("xfConfig*  Saving configuration...");
    File configFile = SPIFFS.open(m_filename, "w");
    if (!configFile) {
      xfHelper::xfdebug("xfConfig*  Error when opening %s for writing.", m_filename);
    }
    if (XFDEBUG_ON) {
      json.printTo(Serial);
      Serial.println("");
    }
    json.printTo(configFile);
    configFile.close();
}

// 
//  Purge all content (i.e. delet all files)
//  
void xfConfigClass::purgeFS() {
  xfHelper::xfdebug("xfConfig*  Purging filesystem...");
  SPIFFS.format();
  xfHelper::xfdebug("xfConfig*  Purged");
  
/*  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    xfHelper::xfdebug("xfConfig*  purgeFS - Removing %s", dir.fileName().c_str());
    SPIFFS.remove(dir.fileName());
   }
*/
}