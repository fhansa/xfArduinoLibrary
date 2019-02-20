#include "xfOTA.h"

// Global instance
xfOTA xfOta;

xfOTA::xfOTA() 
: logToSerial(false)
{

}

void xfOTA::logMsg(char *msg) {
    if (logToSerial)
        Serial.println(msg);
}   

void xfOTA::setupOTA(char *hostname, char *password, int port)
{
    ArduinoOTA.setPort(port);
    ArduinoOTA.setHostname(hostname);
    if(password) 
        if(strlen(password) > 0)
            ArduinoOTA.setPassword(password);

    ArduinoOTA.onStart([this]() {
        this->logMsg("Start xfOTA");
    });
    ArduinoOTA.onEnd([this]() {
        logMsg("\nEnd xfOTA");
    });
    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        logMsg("xfOTA Progress");
    });
    ArduinoOTA.onError([this](ota_error_t error) {
        logMsg("xfOTA Error");

        if (error == OTA_AUTH_ERROR) logMsg("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) logMsg("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) logMsg("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) logMsg("Receive Failed");
        else if (error == OTA_END_ERROR) logMsg("End Failed");
    });

    ArduinoOTA.begin();
    logMsg("xfOTA is up and running");      
}

void xfOTA::OTALoop() {
    ArduinoOTA.handle();
}
