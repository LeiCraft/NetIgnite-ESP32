#ifndef CMD_REGISTRY_H
#define CMD_REGISTRY_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include "commands/wakeup.h"
#include "commands/heartbeat.h"
#include "commands/get_status.h"

using FuncPtr = JsonDocument (*)(JsonDocument& payload);

struct CommandEntry {
    const char *name;
    FuncPtr handler;
};

class CommandRegistry {
  public:

    static const CommandEntry* getCommand(const char* cmd) {
        if (!cmd) return nullptr;
        for (const auto& entry : registry) {
            if (strcmp(entry.name, cmd) == 0) {
                return &entry;
            }
        }
        return nullptr;
    }

    static bool executeCommand(const char* cmd, JsonDocument& payload, JsonDocument& result) {
        const CommandEntry* entry = getCommand(cmd);
        if (entry) {
            result = entry->handler(payload);
            return true;
        }
        return false;
    }

  private:
    static const inline CommandEntry registry[] = {
        {"WAKEUP", wakeup},
        {"HEARTBEAT", heartbeat},
        {"GET_STATUS", get_status},
    };
};


#endif // CMD_REGISTRY_H