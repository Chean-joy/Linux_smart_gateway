#ifndef __APP_ROUTER_H__
#define __APP_ROUTER_H__

#include "app_device.h"
#include "app_mqtt.h"
#include "app_message.h"
#include "Thirdparty/LOG/log.h"

int app_router_init(void);

int app_router_registerDevice(Device * device);

void app_router_close();

#endif

