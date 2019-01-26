/*
 *  xfConfig.h
 * 
 *  Contains helper methods read and write json configuration to 
 *  a file called ./config.json in arduino's "filesystem"
 * 
 *  Usage:

 * 
 *  TODO:

 */
#ifndef XFCONFIG_H
#define XFCONFIG_H
#include <FS.h>
#include <ArduinoJson.h>  

#define XFCONFIG_DEFAULT_FILENAME "/config.json"
class xfConfigClass {
    protected:
        JsonObject &errorJson();
        char m_filename[50]; 
    public:
        xfConfigClass(char *filename);
        JsonObject &readConfig();
        void saveConfig(JsonObject &json);

        // Delete config file
        void deleteConfig();

        // Delete all files on filesystem 
        void purgeFS(); 

};

extern xfConfigClass xfConfig;

#endif
