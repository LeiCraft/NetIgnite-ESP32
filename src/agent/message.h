#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <limits.h>

class AgentMessage {
public:
    const char* cmd;
    const uint32_t id;
    JsonDocument payload;

    AgentMessage(const char* cmd, uint32_t id, const JsonDocument& payloadDoc)
        : id(id) {
        // Duplicate cmd string to ensure it lives with the object
        this->cmd = strdup(cmd);
        this->payload.set(payloadDoc);
    }

    ~AgentMessage() {
        free((void*)cmd); // free strdup-ed memory
    }

    String encode() const {
        String encodedString = String(cmd) + ":" + String(id) + ":";

        String serializedPayload;
        serializeJson(this->payload, serializedPayload);

        encodedString += serializedPayload;
        return encodedString;
    }

    static AgentMessage* decode(const char* message) {
        if (!message) return nullptr;

        String msg = String(message);
        int firstColon = msg.indexOf(':');
        int secondColon = msg.indexOf(':', firstColon + 1);

        if (firstColon == -1 || secondColon == -1) {
            return nullptr; // Invalid format
        }

        String cmd = msg.substring(0, firstColon);
        String idStr = msg.substring(firstColon + 1, secondColon);
        String json = msg.substring(secondColon + 1);

        char* endPtr = nullptr;
        unsigned long parsedId = strtoul(idStr.c_str(), &endPtr, 10);
        if (endPtr == idStr.c_str() || parsedId > 0xFFFFFFFFUL) {
            return nullptr; // Invalid or out-of-range ID
        }

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, json);
        if (error || !doc.is<JsonObject>()) {
            return nullptr;
        }

        return new AgentMessage(cmd.c_str(), static_cast<uint32_t>(parsedId), doc);
    }

private:
    // Prevent copying to avoid memory issues with strdup/free
    AgentMessage(const AgentMessage&) = delete;
    AgentMessage& operator=(const AgentMessage&) = delete;
};

#endif // MESSAGE_H
