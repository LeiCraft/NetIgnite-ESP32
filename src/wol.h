#ifndef WAKEUP_H
#define WAKEUP_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>

extern WiFiUDP UDP;
extern WakeOnLan WOL;


void wol_wakeup(const char *target_mac_address) {
	WOL.setRepeat(3, 100);
	WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
	WOL.sendMagicPacket(target_mac_address, 9); // Send Wake On Lan packet with the above MAC address. Default to port 9.
}

#endif // WAKEUP_H