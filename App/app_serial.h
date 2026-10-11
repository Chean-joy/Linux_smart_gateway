#ifndef __APP_SERIAL_H__
#define __APP_SERIAL_H__

#include "app_device.h"
#include <termio.h>

typedef enum{
    SERIAL_BAUD_RATE_9600 = '4',
    SERIAL_BAUD_RATE_115200 = '8' 
}SerialBaudRate;

typedef enum{
    STOP_BIT_ONE = 0,
    STOP_BIT_TWO = CSTOPB,
}StopBits;

typedef enum{
    PARITY_NONE = 0,
    PARITY_ODD = PARENB | PARODD,
    PARITY_EVEN = PARENB,
}Parity;

typedef struct SerialDeviceStruct
{
    Device super;
    SerialBaudRate baudrate;
    StopBits stop_bits;
    Parity parity; 
}SerialDevices;

int app_serial_init(SerialDevices *serial_device,char * filename);

int app_serial_setBaudRate(SerialDevices *serial_device,
    SerialBaudRate baud_rate);

int app_serial_setParity(SerialDevices *serial_device,
    Parity parity);

int app_serial_setStopBit(SerialDevices *serial_device,
    StopBits stop_bits);

int app_serial_flush(SerialDevices *serial_device);

int app_serial_setBlockMode(SerialDevices *serial_device,int block_mode);


#endif

