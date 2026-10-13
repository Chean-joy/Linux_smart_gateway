#include "app_router.h"

#include <string.h>

#define MAX_DEVICE_NUM 10

static Device *devices[MAX_DEVICE_NUM];
static int device_num = 0;

static int app_router_mqttCallback(unsigned char *json_str,int len)
{
    unsigned char temp_buf[1024];
    Message message;

    if(app_message_initByJson(&message,json_str,len) < 0)
    {
        return -1;
    }

    int Bin_len = app_message_saveBinary(&message,temp_buf,1024);

    app_message_destroy(&message);
    if(Bin_len < 0)
    {
        log_warn("app_message_saveBinary:Buffer not enough");
        return -1;
    }

    for(int i = 0;i < device_num ; i++)
    {
        if((int)temp_buf[0] == devices[i]->connection_type)
        {
            return app_device_write(devices[i],temp_buf,Bin_len);
        }
    }

    log_warn("app_router_mqttCallback: no devices found!!");
    return -1;
}

static int app_router_deviceCallback(void *ptr,int len)
{
    char temp_buf[1024];
    Message message;
    if(app_message_initByBinary(&message,ptr,len)<0)
    {
        return -1;
    }

    int result = app_message_saveJson(&message,
        temp_buf,sizeof(temp_buf));
    
    app_message_destroy(&message);
    if(result < 0)
    {
        return -1;
    }

    return app_mqtt_send(temp_buf,strlen(temp_buf));
}   

int app_router_init(void)
{
    //init mqtt
    if(app_mqtt_init() < 0)
    {
        return -1;
    }
    app_mqtt_registerRecvCallback(app_router_mqttCallback);
    return 0;
}

int app_router_registerDevice(Device * device)
{
    devices[device_num++] = device;
    app_device_start(device);
    app_device_registerRecvCallback(device,
        app_router_deviceCallback);
    return 0;
}

void app_router_close()
{   
    for(int i =0 ; i < MAX_DEVICE_NUM ; i++)
    {
        app_device_stop(devices[i]);
        app_device_close(devices[i]);
    }
}

