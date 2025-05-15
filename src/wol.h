#ifndef WOL_H
#define WOL_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>

extern WakeOnLan WakeOnLanClient;

class WOL {
  public:

	static void wakeup(const char *target_mac_address, const uint16_t port) {
		WakeOnLanClient.setRepeat(3, 100);
		WakeOnLanClient.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
		WakeOnLanClient.sendMagicPacket(target_mac_address, port); // Send Wake On Lan packet with the above MAC address. Default to port 9.
	}
};

#endif // WOL_H