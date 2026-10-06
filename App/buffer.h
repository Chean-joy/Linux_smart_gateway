#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <pthread.h>

typedef struct BufferStruct
{
    /* data */
    void *ptr;  
    int len;
    int size;
    int start;
    pthread_mutex_t lock;
}Buffer;

/**
 * @brief 缓存初始化方法
 *
 * @param buffer 需要初始化的缓存指针
 * @param total_len 缓存容量
 * @return int 初始化是否成功
 */
int app_buffer_init(Buffer *buffer, int size);

/**
 * @brief 释放缓存
 *
 * @param buffer 缓存指针
 */
void app_buffer_close(Buffer *buffer);

/**
 * @brief 从缓存中读取数据
 *
 * @param buffer 缓存
 * @param ptr 读出的数据指针
 * @param len 读出缓冲度总长度
 * @return int 总共读了多长 0 表示没读到 -1表示异常
 */
int app_buffer_read(Buffer *buffer, void *ptr, int len);

/**
 * @brief 向缓存中写入数据
 *
 * @param buffer 缓存指针
 * @param ptr 要写入的数据指针
 * @param len 要写入的长度
 * @return int 0写入成功 -1写入失败
 */
int app_buffer_write(Buffer *buffer, void *ptr, int len);



#endif

