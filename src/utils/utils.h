#ifndef UTILS_H
#define UTILS_H

#include "utils/logger.h"
#include <WiFi.h>
#include <regex.h>

class Utils {
  public:

    static void setClock() {
        configTime(0, 0, "pool.ntp.org", "time.nist.gov");

        Logger::info(F("Waiting for NTP time sync: "));
        time_t nowSecs = time(nullptr);
        while(nowSecs < 8 * 3600 * 2) {
            delay(500);
            Logger::print(F("."));
            yield();
            nowSecs = time(nullptr);
        }

        Logger::println("");
        struct tm timeinfo;
        gmtime_r(&nowSecs, &timeinfo);
        Logger::info(F("Current UTC time: "));
        Logger::print(asctime(&timeinfo));
    }


    static String utf8ToHexStr(const char* str) {
        String hexStr = "";

        for (int i = 0; str[i] != '\0'; i++) {
            char hex[3];
            snprintf(hex, sizeof(hex), "%02X", str[i]);
            hexStr += hex;
        }

        return hexStr;
    }

    static bool matchRegex(const char* str, const char* pattern) {
        if (!str || !pattern) return false;

        regex_t regex;
        if (regcomp(&regex, pattern, REG_EXTENDED | REG_NOSUB) != 0) {
            // regex compilation failed
            return false;
        }

        int result = regexec(&regex, str, 0, NULL, 0);
        regfree(&regex);
        return result == 0;

    }
};

#endif // UTILS_H