#include "app_task.h"

#include <mqueue.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "Thirdparty/LOG/log.h"

#define MSG_LEN sizeof(struct Taskstruct)

struct Taskstruct{
    Task_type task;
    void *args;
};

static pthread_t *executor_ptr;
static int executors_count = 0;

static mqd_t mq;

static void * app_task_executor(void *argv)
{
    int count = argv;
    log_info("Executor %d start !\n",count);
    struct Taskstruct task_struct;

    while (1)
    {
        if(mq_receive(mq,(char * )&task_struct,MSG_LEN,0) < 0)
        {
            continue;
        }
        task_struct.task(task_struct.args);
    }

    return NULL;
}


int app_task_init(int executors)
{
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_LEN;
    mq = mq_open("/gateway-mqueue",O_RDWR | O_CREAT,0664, &attr);
    if(mq < 0)
    {
        log_error("mq_open_error");
        // return -1;
        goto EXIT;
    }

    executors_count = executors;
    executor_ptr = malloc(executors_count * sizeof(pthread_t));
    if(!executor_ptr)
    {
        log_error("Not enough memroy for manager");
        // return -1;
        goto MQ_EXIT;
    } 
    memset(executor_ptr,0,sizeof(executors_count * sizeof(pthread_t)));
    int i;
    for (i = 0; i < executors_count; i++)
    {   
        int res = pthread_create(executor_ptr + i,NULL,app_task_executor,(void *)i);
        if(res < 0)
        {   
            log_warn("mq_create error");
            goto FREE_EXIT;
        }
        
    }
    return 0;
FREE_EXIT:
    for(i = 0; i < executors_count ; i++)
    {
        if(executor_ptr[i])
        {
            pthread_cancel(executor_ptr[i]);
            pthread_join(executor_ptr[i],NULL);
        }
    }
    free(executor_ptr);
MQ_EXIT:
    mq_unlink("/gateway-mqueue");
EXIT:
    return -1;
}

int app_task_registerTask(Task_type task, void *args)
{
    struct Taskstruct task_struct ={
        .task = task,
        .args = args,
    };
    int result = mq_send(mq,(char *)&task_struct,MSG_LEN,0);

    if(result < 0)
    {
        log_warn("send error!!!\n");
        return -1;
    }

    log_debug("Task %p register",task);
    return 0;
}

void app_task_wait(void)
{
    for(int i = 0; i < executors_count ; i++)
    {
        if(executor_ptr[i])
        {
            pthread_join(executor_ptr[i],NULL);
        }
    }
    free(executor_ptr);
    mq_unlink("/gateway-mqueue");
    log_info("Task Cancled !!!\n");
}

void app_task_close(void)
{
    log_info("Task Closed...\n");
    for(int i = 0; i < executors_count ; i++)
    {
        if(executor_ptr[i])
        {
            pthread_cancel(executor_ptr[i]);
        }
    }
    
}
