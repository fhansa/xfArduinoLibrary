#include "xfMQTTDevice.h"

// Instance of WiFiClient to be used by MQTT
WiFiClient  g_wifi;

// Topics and payloads 

// Base MQTTDevice
#define T_AVAILABILITY    "~/availability"
#define PAYLOAD_OFFLINE   "offline"
#define PAYLOAD_ONLINE    "online"

// Binary sensor (base)
#define T_STATE         "~/state"
#define PAYLOAD_ON              "on"
#define PAYLOAD_OFF             "off"

// Light (BinarySensor)
#define T_STATE_COMMAND         "~/set"
#define T_BRIGHTNESS_COMMAND    "~/bri/set"
#define T_BRIGHTNESS_STATE      "~/bri/state"
#define DEF_BRIGHTNESS_SCALE    255
#define T_COLOR_TEMP_COMMAND    "~/clr_temp/set"
#define T_COLOR_TEMP_STATE      "~/clr_temp/state"
#define T_EFFECT_COMMAND        "~/fx/set"
#define T_EFFECT_STATE          "~/fx/state"
#define T_HUE_SAT_COMMAND        "~/hs/set"
#define T_HUE_SAT_STATE          "~/hs/state"
#define T_RGB_COMMAND            "~/rgb/set"
#define T_RGB_STATE              "~/rgb/state"
#define T_WHITE_VALUE_COMMAND    "~/whit_val/set"
#define T_WHITE_VALUE_STATE      "~/whit_val/state"
#define DEF_WHITE_VALUE_SCALE    255
#define T_XY_COMMAND            "~/xy/set"
#define T_XY_STATE              "~/xy/state"

// --------------------------------------------------------------------------
//  xfMQTTDevice
//

// Constructor to initialize attributes
xfMQTTDevice::xfMQTTDevice(char *baseTopic, char *deviceName, char *discoveryPrefix) {
  this->name = deviceName;
  this->mqttBaseTopic = baseTopic;
  this->discoveryPrefix = discoveryPrefix;
  // TODO:  Make unique id random
  unique_id = strcat(DEF_UNIQUE_ID_PREFIX, "1002"); 
}

// Create and setup MQTT
void xfMQTTDevice::setupMQTT(char *host, int port, char *username, char *password) {
  Serial.println("xfMQTTDevice::setupMQTT");
  m_mqtt = new xfMQTT();
  m_mqtt->setup(host, port, g_wifi, true, (const char*)unique_id.c_str(), T_AVAILABILITY, PAYLOAD_OFFLINE);
  if (m_mqtt->connected()) { 
    // Auto discover and publish availability
    publishDiscovery();
    publishAvailability();
  }
}

// Wrapper for mqtt-loop
void xfMQTTDevice::messageLoop() {
  m_mqtt->loop();
}

// getDeviceType - return a non valid type
String xfMQTTDevice::getDeviceType() {
  return "error";
}

// Populate the discovery json payload
void xfMQTTDevice::getDiscoveryPayload(JsonObject& json) {
  json["uniq_id"] = this->name + "1102";
  json["name"] = this->name;
  json["~"] = mqttBaseTopic + "/" + name;
  json["avty_t"] = T_AVAILABILITY;
  json["pl_avail"] = PAYLOAD_ONLINE;                  
  json["pl_not_avail"] = PAYLOAD_ONLINE;
}

// Publlish discovery config
void xfMQTTDevice::publishDiscovery() {
  Serial.print("Discovery:");

  String discoveryTopic = discoveryPrefix + "/"+ getDeviceType() + "/" + name + "/config";
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  getDiscoveryPayload(json);

  char buf[1024];
  json.printTo(buf);
  Serial.println(buf);
  if(!m_mqtt->publish(discoveryTopic.c_str(), buf, true)) {
    Serial.print("ERROR");
    Serial.println(m_mqtt->state());
  }
}

void xfMQTTDevice::publishAvailability() {
  m_mqtt->publish(T_AVAILABILITY, PAYLOAD_ONLINE, true);
}

// --------------------------------------------------------------------------
//  xfMQTTBinarySensor
//
#define T_STATE_COMMAND         "~/state"
#define PAYLOAD_ON              "on"
#define PAYLOAD_OFF             "off"

xfMQTTBinarySensor::xfMQTTBinarySensor(char *baseTopic, char *deviceName, char *discoveryPrefix)
: xfMQTTDevice(baseTopic, deviceName, discoveryPrefix) {
  // 
}

// getDeviceType 
String xfMQTTBinarySensor::getDeviceType() {
  return "binary_sensor";
}
// Populate the discovery json payload
void xfMQTTBinarySensor::getDiscoveryPayload(JsonObject& json) {
  xfMQTTDevice::getDiscoveryPayload(json);
  json["stat_t"] = T_STATE_COMMAND;
  json["pl_on"] = PAYLOAD_ON;
  json["pl_off"] = PAYLOAD_OFF;
}

// changeState of sensor - called by client
void xfMQTTBinarySensor::changeState(bool state) {
  this->state = state;
  this->m_mqtt->publish(T_STATE_COMMAND, (this->state ? PAYLOAD_ON : PAYLOAD_OFF), true);
}


//--------------------------------------------------------------------------
//  xfMQTTLight
//
#define T_STATE_COMMAND         "~/set"

#define T_BRIGHTNESS_COMMAND    "~/bri/set"
#define T_BRIGHTNESS_STATE      "~/bri/state"
#define DEF_BRIGHTNESS_SCALE    255

#define T_COLOR_TEMP_COMMAND    "~/clr_temp/set"
#define T_COLOR_TEMP_STATE      "~/clr_temp/state"

#define T_EFFECT_COMMAND        "~/fx/set"
#define T_EFFECT_STATE          "~/fx/state"

#define T_HUE_SAT_COMMAND        "~/hs/set"
#define T_HUE_SAT_STATE          "~/hs/state"

#define T_RGB_COMMAND            "~/rgb/set"
#define T_RGB_STATE              "~/rgb/state"

#define T_WHITE_VALUE_COMMAND    "~/whit_val/set"
#define T_WHITE_VALUE_STATE      "~/whit_val/state"
#define DEF_WHITE_VALUE_SCALE    255

#define T_XY_COMMAND            "~/xy/set"
#define T_XY_STATE              "~/xy/state"

// Constructor
xfMQTTLight::xfMQTTLight(char *baseTopic, char *deviceName, char *discoveryPrefix)
: xfMQTTBinarySensor(baseTopic, deviceName, discoveryPrefix) {
  effectList = "";
  brightness_scale = DEF_BRIGHTNESS_SCALE;
  whiteValueScale = DEF_WHITE_VALUE_SCALE;
}

// getDeviceType 
String xfMQTTLight::getDeviceType() {
  return "light";
}

// Populate the discovery json payload
void xfMQTTLight::getDiscoveryPayload(JsonObject& json) {
  xfMQTTBinarySensor::getDiscoveryPayload(json);

  // Inherited from binary sensor (e.g. availability and state is already rixed)

  json["cmd_t"] = T_STATE_COMMAND;

  // Only configure enable functions
  if (enableBrightness) {
    json["bri_cmd_t"] = T_BRIGHTNESS_COMMAND;
    json["bri_scl"] = brightness_scale;
    json["bri_stat_t"] = T_BRIGHTNESS_STATE;
  }

  if(enableColorTemp) {
    json["clr_temp_cmd_t"] = T_COLOR_TEMP_COMMAND;
    json["clr_temp_stat_t"] = T_COLOR_TEMP_STATE;
  }

  if(enableEffects) {
    json["fx_cmd_t"] = T_EFFECT_COMMAND;
    json["fx_stat_t"] = T_EFFECT_STATE;
    json["fx_list"] = effectList;
  }

  if(enableHueSaturation) {
    json["hs_command_topic"] = T_HUE_SAT_COMMAND;
    json["hs_state_topic"] = T_HUE_SAT_STATE;
  } 

  if(enableRGB) {
    json["rgb_cmd_t"] = T_RGB_COMMAND;
    json["rgb_stat_t"] = T_RGB_STATE;
  }

  if(enableWhiteValue) {
    json["whit_val_cmd_t"] = T_WHITE_VALUE_COMMAND;
    json["whit_val_stat_t"] = T_WHITE_VALUE_STATE;
    json["whit_val_scl"] = whiteValueScale;
  }

  if(enableXY) {
    json["xy_cmd_t"] = T_XY_COMMAND;
    json["xy_stat_t"] = T_XY_STATE;
  }
}
