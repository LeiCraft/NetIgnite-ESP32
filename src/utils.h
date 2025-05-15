#ifndef UTILS_H
#define UTILS_H

#include "logger.h"
#include <WiFi.h>

class Utils {
  public:

    static String utf8ToHexStr(const char* str) {
        String hexStr = "";

        for (int i = 0; str[i] != '\0'; i++) {
            char hex[3];
            sprintf(hex, "%02X", str[i]);
            hexStr += hex;
        }

        return hexStr;
    }
};

#endif // UTILS_H