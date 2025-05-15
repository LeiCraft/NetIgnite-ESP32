#ifndef WOL_H
#define WOL_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>

#include "utils/utils.h"

extern WakeOnLan WakeOnLanClient;

class WOL {
  public:

	static bool wakeup(const char *target_mac_address, const uint16_t port) {

		if (!isValidMacAddress(target_mac_address) || !isValidPort(port)) {
			return false;
		}

		WakeOnLanClient.setRepeat(3, 100);
		WakeOnLanClient.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
		WakeOnLanClient.sendMagicPacket(target_mac_address, port);

		return true;
	}

  private:

	static bool isValidMacAddress(const char *macAddress) {
		if (!macAddress) return false;

		const char *pattern = "^([0-9A-Fa-f]{2})([:-])(?:[0-9A-Fa-f]{2}\2){4}[0-9A-Fa-f]{2}$";
		return Utils::matchRegex(macAddress, pattern);
	}

	static bool isValidPort(uint16_t port) {
		return port > 0 && port <= 65535;
	}

};

#endif // WOL_H