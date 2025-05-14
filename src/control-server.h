#ifndef CONTROL_SERVER_H
#define CONTROL_SERVER_H

#include <WebSocketsClient.h>

#include "logger.h"
#include "wol.h"

extern WebSocketsClient webSocket;

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		Serial.printf("%02X ", *src);
		src++;
	}
	Serial.printf("\n");
}

String utf8ToHexString(const char* str) {
  String hexStr = "";

    // Iterate through each character in the string
    for (int i = 0; str[i] != '\0'; i++) {
        // Convert each character to hexadecimal and append to hexStr
        char hex[3];
        sprintf(hex, "%02X", str[i]);
        hexStr += hex;
    }

    return hexStr;
}

const char *target_mac_address = "F4:6D:04:9C:93:F8";

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Logger::infoln("Websocket Disconnected!");
            break;
        case WStype_CONNECTED:
            {
                Serial.printf("[WSc] Connected to url: %s\n",  payload);

			    // send message to server when Connected
				webSocket.sendTXT("Connected");
            }
            break;
        case WStype_TEXT:
            Serial.printf("[WSc] get text: %s\n", payload);

			// send message to server
			// webSocket.sendTXT("message here");
            break;
        case WStype_BIN:
            Serial.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);

            wol_wakeup(target_mac_address);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
    }

}

void setupWebSocket() {
	// webSocket.beginSSL("192.168.0.123", 81);

    String url = "/api/control-service?id=" + utf8ToHexString("1") + "&secret=" + utf8ToHexString("123456");

    webSocket.begin("192.168.2.210", 3000, url);
    webSocket.onEvent(webSocketEvent);
}

#endif // CONTROL_SERVER_H