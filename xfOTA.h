/*
 *  xfOTA.h
 * 
 *  Contains helper methods to setup ArduinoOTA. 
 *  Purpose is to setup and use OTA with two single line calls. 
 * 
 *  Usage:
 * 
 *      call xfOta.setup(hostname, password, port) from setup
 *      Call xfOta.OTALoop from Loop
 * 
 *  TODO:
 *    - extend logging
 *    - enable logging to mqtt
 */
#ifndef XFOTA_H
#define XFOTA_H

#include <ArduinoOTA.h>

class xfOTA {
  protected:
    void logMsg(char *msg);
  public:
    xfOTA();
    // Flag to log OTA to Serial or not
    bool logToSerial; 

    // setup OTA
    void setupOTA(char *hostname, char *password, int port = 6266);
    
    // Always call this from arduino loop()
    void OTALoop();
};

// Make global single instance
extern xfOTA xfOta;
#endif