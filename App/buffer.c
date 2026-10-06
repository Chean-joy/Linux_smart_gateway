#include "buffer.h"
#include <string.h>
#include <stdlib.h>
#include "Thirdparty/LOG/log.h"

static pthread_mutex_t lock_initializer = PTHREAD_MUTEX_INITIALIZER;


int app_buffer_init(Buffer *buffer, int size)
{
    buffer->ptr = malloc(sizeof(size));
    if(buffer->ptr == NULL)
    {
        log_warn("Not Enough memory for buffer %p",buffer);
        return -1;
    }
    memcpy(&buffer->lock,&lock_initializer,sizeof(pthread_mutex_t));
    buffer->size = size;
    buffer->len = 0;
    buffer->start = 0;
    log_info("Buffer %p createed",buffer);
    return 0;
}

void app_buffer_close(Buffer *buffer)
{
    if(buffer->ptr)
    {
        free(buffer->ptr);
        buffer->ptr = NULL;
    }
    buffer->size = 0;
    buffer->len = 0;
}

int app_buffer_read(Buffer *buffer, void *ptr, int len)
{
    //get how len ??
    if(len > buffer->len)
    {
        len = buffer->len;
    }

    if(len == 0)
    {
        return 0;
    }

    if(buffer->start+len <= buffer->size)
    {
        //read 1
        memcpy(ptr,(buffer->ptr + buffer->start),len);

        buffer->start += len;
    }
    else
    {   //read 2
        int first_len = buffer->size - buffer->start;
        memcpy(ptr,buffer->ptr + buffer->start,first_len);

        memcpy(ptr+first_len,buffer->ptr,(len - first_len));

        buffer->start = (len - first_len);
    }   

    return len;
}

int app_buffer_write(Buffer *buffer, void *ptr, int len)
{
    return 0;
}
