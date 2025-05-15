#ifndef WAKEUP_H
#define WAKEUP_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "utils/wol.h"

JsonDocument wakeup(JsonDocument &input) {

    JsonDocument response;

    if (!input.containsKey("macAddress") || !input["macAddress"].is<const char *>() || 
        !input.containsKey("port") || !input["port"].is<uint16_t>()
    ) {
        response["status"] = "ERROR";
        response["message"] = "Missing or invalid payload";
        return response;
    }

    const char *macAddress = input["macAddress"];
    uint16_t port = input["port"];

    const bool result = WOL::wakeup(macAddress, port);
    if (!result) {
        response["status"] = "ERROR";
        response["message"] = "Wakeup failed: Invalid MAC address or port";
        return response;
    }

    response["status"] = "OK";
    response["message"] = "Wakeup successful";

    return response;
}

#endif // WAKEUP_H