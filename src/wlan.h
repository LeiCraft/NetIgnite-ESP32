#ifndef WLAN_H
#define WLAN_H

#include <WiFi.h>

#include "logger.h"

class WLAN {
  public:
    static void setup(
		const char *ssid,
		const char *password,
        IPAddress local_IP,
		IPAddress gateway,
        IPAddress subnet_mask,
        IPAddress primary_DNS = (uint32_t)0,
        IPAddress secondary_DNS = (uint32_t)0
	) {
        Logger::infoln("Connecting to Wifi...");

        if (!WiFi.config(local_IP, gateway, subnet_mask, primary_DNS, secondary_DNS)) {
            Logger::errorln("STA Failed to configure Wifi");
        } else {
            Logger::infoln("STA Configured");
        }

        Logger::infoln("Connecting to " + String(ssid) + "...");
        WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
        }
        // Print local IP address and start web server
        Logger::infoln("Wifi connected.");
        Logger::infoln("IP address: " + WiFi.localIP().toString());
    }

};

#endif // WLAN_H