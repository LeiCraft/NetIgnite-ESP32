#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <limits.h>
#include <cstring>

class AgentMessage {
  public:

    const String cmd;
    const uint32_t id;
    JsonDocument payload;

    AgentMessage(const String& cmd, uint32_t id, const JsonDocument& payloadDoc)
        : cmd(cmd), id(id) {
        this->payload.set(payloadDoc);
    }

    String encode() const {
        String encodedString = cmd + ":" + String(id) + ":";

        String serializedPayload;
        serializeJson(payload, serializedPayload);

        encodedString += serializedPayload;
        return encodedString;
    }

    static bool decode(const String& message, AgentMessage*& result) {

        result = nullptr;

        if (message.length() == 0) return false;

        int firstColon = message.indexOf(':');
        int secondColon = message.indexOf(':', firstColon + 1);

        if (firstColon == -1 || secondColon == -1) {
            return false;
        }

        String cmdStr = message.substring(0, firstColon);
        String idStr = message.substring(firstColon + 1, secondColon);
        String jsonStr = message.substring(secondColon + 1);

        char* endPtr = nullptr;
        unsigned long parsedId = strtoul(idStr.c_str(), &endPtr, 10);
        if (endPtr == idStr.c_str() || parsedId > 0xFFFFFFFFUL) {
            return false;
        }

        JsonDocument doc;  // or appropriate size
        DeserializationError error = deserializeJson(doc, jsonStr);
        if (error || !doc.is<JsonObject>()) {
            return false;
        }

        result = new AgentMessage(cmdStr, static_cast<uint32_t>(parsedId), doc);
        return true;
    }

};

#endif // MESSAGE_H
