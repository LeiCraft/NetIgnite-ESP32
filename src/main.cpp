#include <Arduino.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <WebSocketsClient.h>

#include "config/config.h"
#include "utils/logger.h"
#include "utils/wlan.h"
#include "utils/utils.h"
#include "agent/agent.h"
#include "utils/icmp.h"

WebSocketsClient webSocket;
WiFiUDP UDPClient;
WakeOnLan WakeOnLanClient(UDPClient);

// TaskHandle_t pingTaskHandle;

WLANConfig wlanConfig = {
	C_WIFI_SSID,
	C_WIFI_PASSWORD,
	C_WIFI_LOCAL_IP,
	C_WIFI_GATEWAY_IP,
	C_WIFI_SUBNET_MASK,
	C_WIFI_PRIMARY_DNS,
	C_WIFI_SECONDARY_DNS
};

AgentConfig agentConfig = {
	C_CONTROL_SERVER_HOST,
	C_CONTROL_SERVER_PORT,
	C_CONTROL_SERVER_USE_SSL,
	C_CONTROL_SERVER_AUTH_ID,
	C_CONTROL_SERVER_AUTH_SECRET
};


void setup()
{
	Logger::init(115200);
	Logger::setLevel(LogLevel::INFO);

	Logger::infoln("Starting...");
	
	WLAN::setup(wlanConfig);

	delay(1000);

	Utils::setClock();

	Agent::setup(agentConfig);

	// xTaskCreatePinnedToCore(pingTask, "Ping", 4096, NULL, 1, &pingTaskHandle, 1);
}
 

void loop()
{
	WLAN::loop();
	Agent::loop();
}