#ifndef WOL_H
#define WOL_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include "utils/inputValidation.h"
#include "utils/utils.h"

extern WakeOnLan WakeOnLanClient;

class WOL {
  public:

	static bool wakeup(const char *target_mac_address, const uint16_t port) {

		if (!InputValidation::macAddress(target_mac_address) || !InputValidation::port(port)) {
			return false;
		}

		WakeOnLanClient.setRepeat(3, 100);
		WakeOnLanClient.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
		WakeOnLanClient.sendMagicPacket(target_mac_address, port);

		return true;
	}

};

#endif // WOL_H