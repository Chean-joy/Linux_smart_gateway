#include "app_device.h"

#include <stdlib.h>
#include "Thirdparty/LOG/log.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#define BUFFER_LEN (16*1024)

static void *app_device_background(void *argv)
{
    unsigned char temp_buf[1024];
    Device* device = argv;
    while (1)
    {
        int buf_len = read(device->fd,temp_buf,1024);
        if(buf_len < 0)
        {
            log_warn("read device data error");
            continue;
        }

        if(device->vbtr->post_read)
        {
            device->vbtr->post_read(device,temp_buf,&buf_len);
        }

        if(buf_len > 0)
        {
            app_buffer_write(device->recv_buffer,temp_buf,buf_len);
        }
        app_task_registerTask(device->vbtr->recv_task,device);
    }
}


static void app_device_defalutSendTask(void * ARGV)
{
    unsigned char temp_buf[1024];
    int buf_len = 0;
    Device * device = ARGV;
    app_buffer_read(device->send_buffer,temp_buf,3);
    app_buffer_read(device->send_buffer,temp_buf + 3,
        temp_buf[0] + temp_buf[1]);
    buf_len = 3 + temp_buf[2] + temp_buf[1];
    
    if(device->vbtr->pre_write)
    {
        device->vbtr->pre_write(device,temp_buf,&buf_len);
    }

    if(&buf_len > 0)
    {
        write(device->fd,temp_buf,buf_len);
    }
}

static void app_device_defalutRecvTask(void *ARGV)
{
    unsigned char temp_buf[1024];
    Device* device = ARGV;

    app_buffer_read(device->recv_buffer,temp_buf,3);
    app_buffer_read(device->recv_buffer,temp_buf+3,
        temp_buf[1] + temp_buf[2]);
    int buf_len = 3 + temp_buf[1] + temp_buf[2];

    while (device->vbtr->recv_callback(temp_buf,buf_len) < 0)
    {
        usleep(100000);
    }
    

}

int app_device_init(Device *device, char *filename)
{
    device->filename = malloc(strlen(filename)+1);
    if(!device->filename)
    {
        log_warn("Not Enought memroy for device %s",filename);
        goto DEVICE_EXIT;
    }
    device->vbtr = malloc(sizeof(struct VTable));
    if(!device ->vbtr)
    {
        log_warn("Not enough Memroy for device %s",filename);
        goto DEVICE_FILENAME_EXIT;
    }
    device->recv_buffer = malloc(sizeof(Buffer));
    if(!device->recv_buffer)
    {
        log_warn("Not enough Memroy for device %s",filename);
        goto DEVICE_VTABLE_EXIT;
    }
    device->send_buffer = malloc(sizeof(Buffer));
    if(!device->send_buffer)
    {
        log_warn("Not enough Memroy for device %s",filename);
        goto DEVICE_RECV_BUFFER_EXIT;
    }
    strcpy(device->filename,filename);
    device->fd = open(device->filename,O_RDWR | O_NOCTTY);  
    if(device->fd < 0)
    {
        log_warn("Device Open failed!!!");
        goto DEVICE_SEND_BUFF_EXIT;
    }
    device->connection_type = CONNECTION_TYPE_NONE; 
    if(app_buffer_init(device->recv_buffer,BUFFER_LEN)<0)
    {
        log_warn("recv_buffer init fail!!!\n");
        goto DEVICE_OPEN_FAIL;
    }

    if(app_buffer_init(device->send_buffer,BUFFER_LEN)<0)
    {
        log_warn("send_buffer init fail!!!!\n");
        goto DEVICE_RECV_INIT_FAIL;
    }
    device->is_running = 0;
    device->vbtr->background_task = app_device_background;
    device->vbtr->send_task = app_device_defalutSendTask;
    device->vbtr->recv_task = app_device_defalutRecvTask;
    device->vbtr->pre_write = NULL;
    device->vbtr->post_read = NULL;
    device->vbtr->recv_callback = app_device_registerRecvCallback;
    return 0;
DEVICE_RECV_INIT_FAIL:
    app_buffer_close(device->recv_buffer);
DEVICE_OPEN_FAIL:
    close(device->fd);
DEVICE_SEND_BUFF_EXIT:
    free(device->send_buffer);
DEVICE_RECV_BUFFER_EXIT:
    free(device->recv_buffer);
DEVICE_VTABLE_EXIT:
    free(device->vbtr);
DEVICE_FILENAME_EXIT:
    free(device->filename);
DEVICE_EXIT:
    return -1;
}

int app_device_write(Device *device, void *ptr, int len)
{
    if(app_buffer_write(device->send_buffer,ptr,len) < 0)
    {
        return -1; //write failed
    }

    if(app_task_registerTask(device->vbtr->send_task,device) < 0)
    {
        return -1;
    }
    return 0;
}

int app_device_close(Device *device)
{
    app_buffer_close(device->send_buffer);
    app_buffer_close(device->recv_buffer);
    close(device->fd);
    free(device->send_buffer);
    free(device->recv_buffer);
    free(device->vbtr);
    free(device->filename);
    return 0;
}

void app_device_registerRecvCallback(Device *device,int (*recv_callback)(void *,int))
{
    device->vbtr->recv_callback = recv_callback;
}

int app_device_start(Device *device)
{
    if(device->is_running)
    {
        return -1;
    }
    int res = pthread_create(&device->background_thread,
    NULL,device->vbtr->background_task,device);

    if(res < 0) return -1;
    device->is_running = 1;
    return 0;
}

void app_device_stop(Device *device)
{
    if(device->is_running)
    {
        pthread_cancel(device->background_thread);
        pthread_join(device->background_thread,NULL);
        device->is_running = 0;
    }
}

