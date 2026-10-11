#include "app_serial.h"

#include "Thirdparty/LOG/log.h"

#include <unistd.h>
#include <termios.h>

static int app_serial_setCS8Bits(SerialDevices *serial_device)
{
    struct termios options;
    if(tcgetattr(serial_device->super.fd,&options) != 0)
    {
        return -1;
    }

    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    return tcsetattr(serial_device->super.fd,TCSAFLUSH,&options);
}

static int app_serial_setRAW(SerialDevices *serial_device)
{
    struct termios options;
    if(tcgetattr(serial_device->super.fd,&options) != 0)
    {
        return -1;
    }

    cfmakeraw(&options);

    return tcsetattr(serial_device->super.fd,TCSAFLUSH,&options);
}

int app_serial_init(SerialDevices *serial_device,char * filename)
{
    if(app_device_init(&serial_device->super,filename))
    {
        log_warn("device init fail!!!");
        return -1;
    }

    app_serial_setCS8Bits(serial_device);
    app_serial_setBaudRate(serial_device,serial_device->baudrate);
    app_serial_setParity(serial_device,serial_device->parity);
    app_serial_setStopBit(serial_device,serial_device->stop_bits);
    app_serial_setRAW(serial_device);
    app_serial_setBlockMode(serial_device,0);
    return tcflush(serial_device->super.fd,TCIOFLUSH);
}

int app_serial_setBaudRate(SerialDevices *serial_device, SerialBaudRate baud_rate)
{
    struct termios options;
    if(tcgetattr(serial_device->super.fd,&options) != 0)
    {
        return -1;
    }

    switch (baud_rate)
    {
    case SERIAL_BAUD_RATE_9600:
        cfsetispeed(&options,B9600);
        cfsetospeed(&options,B9600);
        break;
    case SERIAL_BAUD_RATE_115200:
        cfsetispeed(&options,B115200);
        cfsetospeed(&options,B115200);
        break;
    default:
        break;
    }

    return tcsetattr(serial_device->super.fd,TCSAFLUSH,&options);
}

int app_serial_setParity(SerialDevices *serial_device, Parity parity)
{
    struct termios options;
    if(tcgetattr(serial_device->super.fd,&options) != 0)
    {
        return -1;
    }

    options.c_cflag &= ~(PARENB | PARODD);
    options.c_cflag |= parity;

    return tcsetattr(serial_device->super.fd,TCSAFLUSH,&options);
}

int app_serial_setStopBit(SerialDevices *serial_device, StopBits stop_bits)
{
    struct termios options;
    if(tcgetattr(serial_device->super.fd,&options) != 0)
    {
        return -1;
    }

    options.c_cflag &= ~CSTOPB;
    options.c_cflag |= stop_bits;

    return tcsetattr(serial_device->super.fd,TCSAFLUSH,&options);
}

int app_serial_flush(SerialDevices *serial_device)
{
    return tcflush(serial_device->super.fd,TCIOFLUSH);
}

int app_serial_setBlockMode(SerialDevices *serial_device,int block_mode)
{
    struct termios options;
    if(tcgetattr(serial_device->super.fd,&options) != 0)
    {
        return -1;
    }

    if(block_mode)
    {
        options.c_cc[VTIME] = 0;
        options.c_cc[VMIN] = 1;
    }
    else
    {
        options.c_cc[VTIME] = 5;
        options.c_cc[VMIN] = 0;
    }

    return tcsetattr(serial_device->super.fd,TCSAFLUSH,&options);
}


