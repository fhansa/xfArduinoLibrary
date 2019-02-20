#include "xfConfig.h"

// Global instance 
xfConfigClass xfConfig(XFCONFIG_DEFAULT_FILENAME);
StaticJsonBuffer<512> jsonBuffer;

// 
//  Create error json object
// 
JsonObject &xfConfigClass::errorJson() {
    DynamicJsonBuffer jsonBuffer;
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

  Serial.println("Reading configuration");
  Serial.println(m_filename);
  if(!SPIFFS.begin()) {
    Serial.println("ERROR - Cannot mount FS.");
    return errorJson();
  }

  File configFile = SPIFFS.open(m_filename, "r");
  if(!configFile) {
    Serial.print("ERROR - Cannot open ");
    Serial.println(m_filename);
    return errorJson();
  }

  // Read config.json into memory 
  size_t fileSize = configFile.size();
  std::unique_ptr<char []> buf(new char[fileSize]);
  configFile.readBytes(buf.get(), fileSize);

  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if(!json.success()) {
    Serial.println("ERROR - Cannot parse json");
    return errorJson();
  }
  json.printTo(Serial);
  Serial.println("");

  configFile.close()    ;

  return json;
}

//
//  Save config 
//
void xfConfigClass::saveConfigFromJson(JsonObject &json) {
    Serial.println("Saving config");
    File configFile = SPIFFS.open(m_filename, "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }
    json.printTo(configFile);
    configFile.close();
}
