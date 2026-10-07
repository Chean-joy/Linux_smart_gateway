#include "App/app_mqtt.h"

#include <unistd.h>
#include <assert.h>
#include <string.h>


static int receive_flag = 0;

int recvcallback(void * str,int len)
{
    receive_flag = 1;
    assert(memcmp(str,"Hello World!",len) == 0);
    return 0;
}


int main()
{
    if(app_mqtt_init() != 0)
    {
        printf("MQTT init failed!\n");
        return -1; 
    }


    app_mqtt_registerRecvCallback(recvcallback);

    app_mqtt_send("Hello World!",12);

    sleep(1);

    assert(receive_flag == 1);

    app_mqtt_close();

    return 0;
}
