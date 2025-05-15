#ifndef WLAN_H
#define WLAN_H

#include <WiFi.h>

#include "logger.h"

struct WLANConfig {
    const char* ssid;
    const char* password;
    IPAddress local_IP;
    IPAddress gateway;
    IPAddress subnet_mask;
    IPAddress primary_DNS = (uint32_t) 0;
    IPAddress secondary_DNS = (uint32_t) 0;
};

class WLAN {
  public:
    static void setup(const WLANConfig& config) {

        ssid = config.ssid;
        password = config.password;

        WiFi.disconnect(true);
        delay(1000);

        Logger::infoln("Connecting to Wifi...");

        if (!WiFi.config(config.local_IP, config.gateway, config.subnet_mask, config.primary_DNS, config.secondary_DNS)) {
            Logger::errorln("STA Failed to configure Wifi");
        } else {
            Logger::infoln("STA Configured");
        }

        WiFi.onEvent(onWiFiConnect, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent(onWiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.onEvent(onWiFiDisconnect, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

        Logger::infoln("Connecting to " + String(ssid) + "...");
        WiFi.begin(ssid, password);
    }

    static void loop() {
        if (shouldReconnect && millis() - lastDisconnectTime >= reconnectInterval) {
            Logger::infoln("Trying to Reconnect to WiFi...");
            WiFi.begin(ssid, password);
            shouldReconnect = false;  // reset flag
        }
    }

  private:
    static inline const char* ssid = nullptr;
    static inline const char* password = nullptr;

    static inline unsigned long lastDisconnectTime = 0;
    static inline bool shouldReconnect = false;
    static inline const unsigned long reconnectInterval = 60000; // 60 seconds


    static void onWiFiConnect(WiFiEvent_t event, WiFiEventInfo_t info) {
        Logger::infoln("Wifi connected to " + String(ssid) + ". Waiting for IP...");
    }

    static void onWiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
        Logger::infoln("Wifi connected.");
        Logger::infoln("IP address: " + WiFi.localIP().toString());
    }

    static void onWiFiDisconnect(WiFiEvent_t event, WiFiEventInfo_t info) {

        Logger::infoln("Disconnected from WiFi!");
        Logger::info("WiFi lost connection. Reason: ");
        Logger::infoln(String(info.wifi_sta_disconnected.reason));
        Logger::infoln("Trying to reconnect in " + String(reconnectInterval / 1000) + " seconds...");

        lastDisconnectTime = millis();
        shouldReconnect = true;
    }

};

#endif // WLAN_H