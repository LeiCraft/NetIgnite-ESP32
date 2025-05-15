#ifndef AGENT_H
#define AGENT_H

#include <WebSocketsClient.h>

#include "logger.h"
#include "wol.h"
#include "utils.h"

extern WebSocketsClient webSocket;

class Agent {
  public:

    static void setup(const String host, const uint16_t port, const bool useSSL, const char* authId, const char* authSecret) {

        webSocket.setReconnectInterval(60000); // 60 seconds

        const String endpoint = "/api/control-service?id=" + Utils::utf8ToHexStr(authId) + "&secret=" + Utils::utf8ToHexStr(authSecret);

        if (useSSL) {
            webSocket.beginSSL(host, port, endpoint);
        } else {
            webSocket.begin(host, port, endpoint);
        }

        webSocket.onEvent(webSocketEvent);
    }

  private:

    static void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

        switch(type) {
            case WStype_DISCONNECTED:

                Logger::infoln("Websocket Disconnected!");
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