#ifndef XFHELPER_HPP
#define XFHELPER_HPP
/*  xfArduinoLibrary 
 *  Copyright Fredrik Santander 2019
 *  https://github.com/fhansa/xfArduinoLibrary
 *  MIT License
 */
#include <stdarg.h>
#include <hardwareSerial.h>

#define XFDEBUG_ON true

/*
 *  Printf-like debug to Serial
 */
class xfHelper {
  public:
  static void xfdebug(char *str, ...) {
    if(XFDEBUG_ON) {
      int i, j, count = 0;

      va_list argv;
      va_start(argv, str);
      for(i = 0, j = 0; str[i] != '\0'; i++) {
        if (str[i] == '%') {
          count++;

          Serial.write(reinterpret_cast<const uint8_t*>(str+j), i-j);

          switch (str[++i]) {
            case 'd': Serial.print(va_arg(argv, int));
              break;
            case 'l': Serial.print(va_arg(argv, long));
              break;  
            case 'f': Serial.print(va_arg(argv, double));
              break;
            case 'c': Serial.print((char) va_arg(argv, int));
              break;
            case 's': Serial.print(va_arg(argv, char *));
              break;
            case '%': Serial.print("%");
              break;
            default:;
          };

          j = i+1;
        }
      };
      va_end(argv);

      if(i > j) {
        Serial.write(reinterpret_cast<const uint8_t*>(str+j), i-j);
      }
      Serial.println("");
    }
  }
};
#endif // XFHELPER_HPP