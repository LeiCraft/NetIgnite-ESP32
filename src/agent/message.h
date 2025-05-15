#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>

class Message {
  public:

    const char* cmd;
    const uint32_t id;
    JsonDocument payload;

    Message(const char* cmd, const uint32_t id, JsonDocument payload)
        : cmd(cmd), id(id), payload(payload) {}

    const char* encode() {
        String encodedString = String(cmd) + ":" + String(id) + ":";
        
        // Serialize JSON to a string
        String serializedPayload;
        serializeJson(this->payload, serializedPayload);
        
        encodedString += serializedPayload;
        return encodedString.c_str();
    }

    static Message* decode(const char* message) {

        String msg = String(message);

        int firstColon = msg.indexOf(':');
        int secondColon = msg.indexOf(':', firstColon + 1);

        if (firstColon == -1 || secondColon == -1) {
            return nullptr; // Invalid format
        }

        String cmd = msg.substring(0, firstColon);
        String id = msg.substring(firstColon + 1, secondColon);
        String json = msg.substring(secondColon + 1);

        // Allocate payload and parse
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, json);
        if (error) {
            return nullptr; // JSON error
        }

        // Create new dynamic Message
        Message* result = new Message(cmd.c_str(), id.c_str(), doc);
        return result;
    }

  private:

};

#endif // MESSAGE_H