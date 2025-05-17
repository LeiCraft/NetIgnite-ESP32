#ifndef AGENT_H
#define AGENT_H

#include <WebSocketsClient.h>

#include "utils/logger.h"
#include "utils/wol.h"
#include "utils/utils.h"
#include "agent/message.h"
#include "agent/cmd_registry.h"

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

        host = config.host;
        port = config.port;
        useSSL = config.useSSL;
        authId = config.authId;
        authSecret = config.authSecret;

        if (strlen(authId) == 0 || strlen(authSecret) == 0) {
            Logger::errorln("Missing credentials!");
            return;
        }

        if (useSSL) {
            webSocket.beginSslWithBundle(host, port, "/api/agent-control-service");
        } else {
            webSocket.begin(host, port, "/api/agent-control-service");
        }

        webSocket.setAuthorization(authId, authSecret);

        webSocket.onEvent(webSocketEvent);
    }

    static void loop() {
        webSocket.loop();
    }

  private:

    static inline const char* host = nullptr;
    static inline uint16_t port = 0;
    static inline bool useSSL = false;
    static inline const char* authId = nullptr;
    static inline const char* authSecret = nullptr;

    static void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {

        switch(type) {
            case WStype_DISCONNECTED:

                webSocket.setReconnectInterval(60000); // 60 seconds

                Logger::infoln("Websocket Disconnected! Retrying in 60 seconds...");
                break;

            case WStype_CONNECTED:

                Logger::infoln("Websocket Connected to " + String(host) + ":" + String(port) + " SSL: " + String(useSSL ? "true" : "false"));
                break;

            case WStype_TEXT: {

                AgentMessage* message = nullptr;

                if (AgentMessage::decode((const char*)payload, message)) {
                    
                    JsonDocument responsePayload;

                    if (!CommandRegistry::executeCommand(message->cmd.c_str(), message->payload, responsePayload)) {
                        responsePayload["status"] = "ERROR";
                        responsePayload["message"] = "Unknown command";
                    }

                    const AgentMessage responseMessage(message->cmd, message->id, responsePayload);
                    
                    String encodedResponse = responseMessage.encode();
                    webSocket.sendTXT(encodedResponse);
                }

                delete message;
                message = nullptr;
                
                break;
            }
            
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