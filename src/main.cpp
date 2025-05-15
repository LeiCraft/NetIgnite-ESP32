#include <Arduino.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <WebSocketsClient.h>

#include "logger.h"
#include "secrets.h"
#include "wlan.h"
#include "agent.h"


WebSocketsClient webSocket;

WiFiUDP UDPClient;
WakeOnLan WakeOnLanClient(UDPClient);


void setup()
{
	Logger::init(115200);
	Logger::setLevel(LogLevel::INFO);

	Logger::infoln("Starting...");

	WLAN::setup(
		C_WIFI_SSID,
		C_WIFI_PASSWORD,
		C_WIFI_LOCAL_IP,
		C_WIFI_GATEWAY_IP,
		C_WIFI_SUBNET_MASK,
		C_WIFI_PRIMARY_DNS,
		C_WIFI_SECONDARY_DNS
	);

	Agent::setup(
		C_CONTROL_SERVER_HOST,
		C_CONTROL_SERVER_PORT,
		C_CONTROL_SERVER_USE_SSL,
		C_CONTROL_SERVER_AUTH_ID,
		C_CONTROL_SERVER_AUTH_SECRET
	);
}

void loop()
{
	webSocket.loop();
}