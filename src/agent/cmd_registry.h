#ifndef CMD_REGISTRY_H
#define CMD_REGISTRY_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include "commands/wakeup.h"

using FuncPtr = JsonDocument (*)(JsonDocument& doc);

struct CommandEntry {
    const char *name;
    FuncPtr handler;
};

constexpr CommandEntry CommandMap[] = {
    {"WAKEUP", wakeup}
};

FuncPtr getCommandFunc(const char* cmd) {
    if (!cmd) return nullptr;
    for (const auto& entry : CommandMap) {
        if (strcmp(entry.name, cmd) == 0) {
            return entry.handler;
        }
    }
    return nullptr;
}


#endif // CMD_REGISTRY_H