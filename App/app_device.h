#ifndef __APP_DEVICE_H__
#define __APP_DEVICE_H__

#include "buffer.h"
#include "app_task.h"
#include "app_message.h"

#include <sys/types.h>

struct VTable;

typedef struct DeviceStruct
{
    struct VTable *vbtr;
    char *filename;
    int fd;
    pthread_t background_thread;
    ConnectionType connection_type;
    Buffer *recv_buffer;    
    Buffer *send_buffer;
    int is_running;
}Device;

struct VTable
{   
    void *(*background_task)(void *);
    Task_type recv_task;
    Task_type send_task;
    int (*post_read) (Device *device,void *ptr,int *len); 
    int (*pre_write) (Device *device,void *ptr,int *len);
    int (*recv_callback)(void *ptr,int len); 
};

int app_device_init(Device *device ,char *filename);

int app_device_write(Device *device,void *ptr,int len);

int app_device_close(Device *device);

void app_device_registerRecvCallback(Device *device,int (*recv_callback)(void *,int));

int app_device_start(Device *device);

void app_device_stop(Device *device);

#endif


