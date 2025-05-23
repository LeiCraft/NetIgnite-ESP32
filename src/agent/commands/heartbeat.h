#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "utils/wol.h"

JsonDocument heartbeat(JsonDocument& input) {

    JsonDocument response;

    // currently no handle needed

    return response;
}

#endif // HEARTBEAT_H