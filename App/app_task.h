#ifndef __APP_TASK_H__
#define __APP_TASK_H__

typedef void (* Task_type)(void *params);

int app_task_init(int executors);

int app_task_registerTask(Task_type task,void *args);

void app_task_wait(void);

void app_task_close(void);


#endif




