#ifndef __APP_BLUETOOTH_H__
#define __APP_BLUETOOTH_H__

#include "app_message.h"
#include "app_device.h"
#include "app_serial.h"
#include "Thirdparty/LOG/log.h"


int app_bluetooth_setConnectionType(SerialDevices *serial_device);

int app_bluetooth_status(SerialDevices *serial_device);

int app_bluetooth_setBaudRate(SerialDevices *serial_device,
    SerialBaudRate serial_baudrate);

int app_bluetooth_reset(SerialDevices *serial_device);

int app_bluetooth_setNetID(SerialDevices *serial_devices,
    char *net_id);

int app_bluetooth_setMAddr(SerialDevices *serial_devices,
    char *net_MAddr);

int app_bluetooth_postRead(Device *devices,
    void *ptr, int *len);

int app_bluetooth_preWrite(Device *devices,
    void *ptr, int *len);

#endif

