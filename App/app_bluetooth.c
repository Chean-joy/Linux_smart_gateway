#include "app_bluetooth.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>

static unsigned char bluetooth_read_buffer[1024];
static int bluetooth_read_buffer_len = 0;

static const unsigned char fix_header[] = {0xF1,0xDD};

static void app_bluetooth_ignore(int n)
{
    bluetooth_read_buffer_len -= n;
    memmove(bluetooth_read_buffer,bluetooth_read_buffer + n,
        bluetooth_read_buffer_len);
}

static int app_bluetooth_waitACK(SerialDevices * serial_device)
{
    usleep(200000);
    unsigned char read_buf[4] ={0};
    read(serial_device->super.fd,read_buf,4);
    if(memcmp(read_buf,"OK\r\n",4)!= 0)
    {
        return -1;
    }
    return 0;
}


int app_bluetooth_setConnectionType(SerialDevices *serial_device)
{
    serial_device->super.connection_type = CONNECTION_TYPE_BLE_MESH;

    app_serial_setBaudRate(serial_device,SERIAL_BAUD_RATE_9600);
    app_serial_setBlockMode(serial_device,0);
    app_serial_flush(serial_device);
    if(app_bluetooth_status(serial_device) == 0)
    {
        if(app_bluetooth_setNetID(serial_device,"1111")<0)
        {
            log_error("BlueTooth:Set ID error");
            return -1;
        }
        if(app_bluetooth_setMAddr(serial_device,"0001")<0)
        {   
            log_error("BlueTooth:Set MAddr error");
            return -1;
        }
        if(app_bluetooth_setBaudRate(serial_device,SERIAL_BAUD_RATE_115200) < 0)
        {
            log_error("BlueTooth:Set BaudRate error");
            return -1;
        }
        if(app_bluetooth_reset(serial_device) < 0)
        {
            log_error("BlueTooth:Reset error");
            return -1;
        }
    }
    app_serial_setBaudRate(serial_device,SERIAL_BAUD_RATE_115200);
    sleep(1);
    app_serial_flush(serial_device);
    app_serial_setBlockMode(serial_device,1);
    return 0;
}

int app_bluetooth_status(SerialDevices *serial_device)
{
    write(serial_device->super.fd,"AT\r\n",4);
    return app_bluetooth_waitACK(serial_device);
}

int app_bluetooth_setBaudRate(SerialDevices *serial_device, SerialBaudRate serial_baudrate)
{
    char buf[] = "AT+BAUD8\r\n";
    buf[7] = serial_baudrate;

    write(serial_device->super.fd,buf,10);

    return app_bluetooth_waitACK(serial_device);
}

int app_bluetooth_reset(SerialDevices *serial_device)
{
    write(serial_device->super.fd,"AT+RESET\r\n",10);
    return app_bluetooth_waitACK(serial_device);
}

int app_bluetooth_setNetID(SerialDevices *serial_devices, char *net_id)
{
    char buf[] = "AT+NETID1111\r\n";
    memcpy(buf+8,net_id,4);
    write(serial_devices->super.fd,buf,4);
    return app_bluetooth_waitACK(serial_devices);
}

int app_bluetooth_setMAddr(SerialDevices *serial_devices,
    char *net_MAddr)
{
    char buf[] = "AT+MADDR0001\r\n";
    memcpy(buf+8,net_MAddr,4);
    write(serial_devices->super.fd,buf,4);
    return app_bluetooth_waitACK(serial_devices);
}

int app_bluetooth_postRead(Device *devices, void *ptr, int *len)
{
    memcpy(bluetooth_read_buffer+bluetooth_read_buffer_len,ptr,*len);
    bluetooth_read_buffer_len += *len;

    if(bluetooth_read_buffer_len < 4)
    {
        *len = 0;
        return 0;
    }

    for (int i = 0; i < bluetooth_read_buffer_len - 3; i++)
    {
        if(memcmp(bluetooth_read_buffer +i,"OK\r\n",4) == 0)
        {
            *len = 0;
            app_bluetooth_ignore(i+4);
            return 0;
        }
        else if(memcmp(bluetooth_read_buffer + i,fix_header,2) == 0)
        {
            app_bluetooth_ignore(i);
            if(bluetooth_read_buffer_len < 
                (bluetooth_read_buffer[2] + 3))
            {
                *len = 0;
                return 0;
            }
            memcpy(ptr,&devices->connection_type,1);
            int temp = 2;
            memcpy(ptr + 1,&temp,1);
            temp = bluetooth_read_buffer[2] - 4;
            memcpy(ptr + 2,&temp,1);
            memcpy(ptr + 3,bluetooth_read_buffer + 3,2);
            memcpy(ptr + 5,bluetooth_read_buffer + 7,
                bluetooth_read_buffer[2] - 4);
            *len = 0;
            return 0;
        }
    }
    *len = 0;
    return 0;
}

int app_bluetooth_preWrite(Device *devices, void *ptr, int *len)
{   
    int temp = 0;

    unsigned char buf[64];

    memcpy(&temp,ptr,1);
    if(temp != (int)(devices->connection_type))
    {
        *len = 0;
        return 0;
    }

    memcpy(&temp,ptr + 1,1);
    if(temp != 2)
    {
        *len = 0;
        return 0;
    }

    memcpy(bluetooth_read_buffer,"AT+MESH",8);
    memcpy(bluetooth_read_buffer + 8,ptr + 3,2);

    memcpy(&temp,ptr + 2,1);
    memcpy(buf + 10,ptr + 5,temp);
    memcpy(buf + 10 + temp,"\r\n",2);
    *len = temp + 12;
    memcpy(bluetooth_read_buffer,buf,*len);
    return 0;
}
