#ifndef SECRETS_H
#define SECRETS_H


#define C_WIFI_SSID "WIFI_SSID"
#define C_WIFI_PASSWORD "WIFI_PASSWORD"

#define C_WIFI_LOCAL_IP IPAddress(192, 168, 1, 32)
#define C_WIFI_GATEWAY_IP IPAddress(192, 168, 1, 1)
#define C_WIFI_SUBNET_MASK IPAddress(255, 255, 255, 0)

#define C_WIFI_PRIMARY_DNS IPAddress(1, 1, 1, 1)
#define C_WIFI_SECONDARY_DNS IPAddress(1, 0, 0, 1)

#define C_CONTROL_SERVER_HOST "example.com"
#define C_CONTROL_SERVER_PORT (uint16_t) 80
#define C_CONTROL_SERVER_USE_SSL (bool) false

#define C_CONTROL_SERVER_AUTH_ID "AUTH_ID"
#define C_CONTROL_SERVER_AUTH_SECRET "AUTH_SECRET"


#endif