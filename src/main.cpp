#include <Arduino.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <WebSocketsClient.h>
#include <WiFiClientSecure.h>

#include "logger.h"
#include "secrets.h"
#include "utils.h"
#include "control-server.h"


WebSocketsClient webSocket;

WiFiUDP UDP;
WakeOnLan WOL(UDP);


void setup()
{
	Logger::init(115200);
	Logger::setLevel(LogLevel::INFO);

	Logger::infoln("Starting...");

	Utils::setupWIFI(
		C_WIFI_SSID,
		C_WIFI_PASSWORD,
		C_WIFI_LOCAL_IP,
		C_WIFI_GATEWAY_IP,
		C_WIFI_SUBNET_MASK,
		C_WIFI_PRIMARY_DNS,
		C_WIFI_SECONDARY_DNS
	);

	setupWebSocket();
}

void loop()
{
	webSocket.loop();
}