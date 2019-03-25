#include "xfMQTTDevice.h"


// Constructor to initialize attributes
xfMQTTDevice::xfMQTTDevice(char *baseTopic, char *deviceName, char *discoveryPrefix) {
  this->name = deviceName;
  this->baseTopic = baseTopic;
  this->discoveryPrefix = discoveryPrefix;
  availability_topic = DEF_AVAILABILITY_TOPIC;
  availability_online = DEF_PAYLOAD_AVAILABILITY_ONLINE;
  availability_offline = DEF_PAYLOAD_AVAILABILITY_OFFLINE;
  unique_id = strcat(DEF_UNIQUE_ID_PREFIX, "1002");
}

// getDeviceType - return a non valid type
String xfMQTTDevice::getDeviceType() {
  return "error";
}

// Populate the discovery json payload
void xfMQTTDevice::populateJson(JsonObject& json) {
  json["uniq_id"] = this->name + "1102";
  json["name"] = this->name;
  json["~"] = baseTopic + "/" + name;
  json["avty_t"] = "~/" + availability_topic;     // availability_topic
  json["pl_avail"] = "online";                    // Payload_available
  json["pl_not_avail"] = "offline";
}

// Publlish discovery config
void xfMQTTDevice::publishDiscovery() {
  Serial.print("Discovery:");

  String discoveryTopic = discoveryPrefix + "/"+ getDeviceType() + "/" + name + "/config";
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  populateJson(json);

  char buf[1024];
  json.printTo(buf);
  Serial.println(buf);
  if(!m_mqtt->publish(discoveryTopic.c_str(), buf, true)) {
    Serial.print("ERROR");
    Serial.println(m_mqtt->state());
  }
}

void xfMQTTDevice::publishAvailability() {
  m_mqtt->publish(availability_topic.c_str(), availability_online.c_str(), true)
}

xfMQTTBinarySensor::xfMQTTBinarySensor(char *baseTopic, char *deviceName, char *discoveryPrefix)
: xfMQTTDevice(baseTopic, deviceName, discoveryPrefix) {
  stateTopic = "~/state";
  payload_state_on = DEF_PAYLOAD_STATE_ON;
  payload_state_off = DEF_PAYLOAD_STATE_OFF;
  Serial.println("BINARY_SENSOR");
}
String xfMQTTBinarySensor::getDeviceType() {
  return "binary_sensor";
}
// Populate the discovery json payload
void xfMQTTBinarySensor::populateJson(JsonObject& json) {
  xfMQTTDevice::populateJson(json);
  json["stat_t"] = stateTopic;
  json["pl_on"] = payload_state_on;
  json["pl_off"] = payload_state_off;
}

void xfMQTTBinarySensor::changeState(bool state) {
  this->state = state;
  this->m_mqtt->publish(stateTopic.c_str(), (this->state ? payload_state_on.c_str() : payload_state_off.c_str()), true);
}