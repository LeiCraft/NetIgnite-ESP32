#ifndef UTILS_H
#define UTILS_H

#include "utils/logger.h"
#include "utils/inputValidation.h"
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

    static char* macToLinkLocalIPv6(const char* macStr) {
        
        if (!macStr) return nullptr;

        // Convert MAC string with colons or dashes to byte array
        uint8_t mac[6];
        if (InputValidation::macAddressColon(macStr)) {
            sscanf(macStr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
        } else if (InputValidation::macAddressDashed(macStr)) {
            sscanf(macStr, "%hhx-%hhx-%hhx-%hhx-%hhx-%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
        } else {
            return nullptr;
        }

        char buffer[40];

        uint8_t eui64[8];
        eui64[0] = mac[0] ^ 0x02;
        eui64[1] = mac[1];
        eui64[2] = mac[2];
        eui64[3] = 0xFF;
        eui64[4] = 0xFE;
        eui64[5] = mac[3];
        eui64[6] = mac[4];
        eui64[7] = mac[5];

        sprintf(buffer,
            "fe80::%02x%02x:%02x%02x:%02x%02x:%02x%02x",
            eui64[0], eui64[1],  // first 16 bits
            eui64[2], eui64[3],  // second 16 bits
            eui64[4], eui64[5],  // third 16 bits
            eui64[6], eui64[7]   // fourth 16 bits
        );

        return strdup(buffer);
    }
};

#endif // UTILS_H