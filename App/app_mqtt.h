#ifndef __APP_MQTT_H__
#define __APP_MQTT_H__

#include <MQTTClient.h>

#define MQTT_SERVER_URL "tcp://192.168.17.128:1883"
#define MQTT_CLIENT_ID "GATEWAY_LINUX"
#define MQTT_TOPIC "TOPIC01"

int app_mqtt_init();

void app_mqtt_close();

void app_mqtt_registerRecvCallback(int (*recv_callback)(void *, int));

int app_mqtt_send(char *json_str,int len);

#endif

