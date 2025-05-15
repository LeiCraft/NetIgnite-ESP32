#ifndef AGENT_H
#define AGENT_H

#include <WebSocketsClient.h>

#include "utils/logger.h"
#include "utils/wol.h"
#include "utils/utils.h"

extern WebSocketsClient webSocket;

struct AgentConfig {
    const char* host;
    uint16_t port;
    bool useSSL;
    const char* authId;
    const char* authSecret;
};

class Agent {
  public:

    static void setup(const AgentConfig& config) {

        if (strlen(config.authId) == 0 || strlen(config.authSecret) == 0) {
            Logger::errorln("Missing credentials!");
            return;
        }

        const String endpoint = "/api/agent-control-service?id=" + Utils::utf8ToHexStr(config.authId) + "&secret=" + Utils::utf8ToHexStr(config.authSecret);

        if (config.useSSL) {
            webSocket.beginSSL(config.host, config.port, endpoint);
        } else {
            webSocket.begin(config.host, config.port, endpoint);
        }

        webSocket.onEvent(webSocketEvent);
    }

    static void loop() {
        webSocket.loop();
    }

  private:

    static void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

        switch(type) {
            case WStype_DISCONNECTED:

                webSocket.setReconnectInterval(60000); // 60 seconds

                Logger::infoln("Websocket Disconnected! Retrying in 60 seconds...");
                break;

            case WStype_CONNECTED:

                Logger::infoln("Websocket Connected! to url: " + String((char*)payload));
                break;

            case WStype_TEXT:

                Logger::infoln("Websocket Message: " + String((char*)payload));

                break;

            case WStype_ERROR:

                Logger::errorln("Websocket Error: " + String((char*)payload));

                break;

            case WStype_BIN:
            case WStype_FRAGMENT_TEXT_START:
            case WStype_FRAGMENT_BIN_START:
            case WStype_FRAGMENT:
            case WStype_FRAGMENT_FIN:
                break;
        }

    }

};

#endif // AGENT_H