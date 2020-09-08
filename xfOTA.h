#ifndef XFOTA_H
#define XFOTA_H
/*  xfArduinoLibrary 
 *  Copyright Fredrik Santander 2019
 *  https://github.com/fhansa/xfArduinoLibrary
 *  MIT License
 * 
 *  Contains helper methods to setup ArduinoOTA. 
 *  Purpose is to setup and use OTA with two single line calls. 
 * 
 *  Usage:
 * 
 *      call xfOta.setup(hostname, password, port) from setup
 *      call xfOta.OTALoop from Loop
 * 
 *  TODO:
 *    - extend logging
 *    - enable logging to mqtt
 */


#include <ArduinoOTA.h>

class xfOTA {
  protected:
    void logMsg(char *msg);
  public:
    xfOTA();
    // Flag to log OTA to Serial or not
    bool logToSerial; 

    // setup OTA by supplying hostname and password
    void setupOTA(char *hostname, char *password, int port = 6266);
    
    // Always call this from arduino loop()
    void OTALoop();
};

// Make global single instance
extern xfOTA xfOta;
#endif //XFOTA_H