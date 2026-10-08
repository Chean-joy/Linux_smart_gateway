#include "App/app_message.h"

#include <assert.h>
#include <string.h>

const unsigned char data[] ={
    0x01,0x02,0x04,0x00,0x01,0xEA,0xAC,0x22,0x88
};

unsigned char json_str[1024] = {0};
unsigned char result[9];

int main(void)
{
    Message message;
    app_message_initByBinary(&message,(char *)data,sizeof(data));
    app_message_saveJson(&message,json_str,sizeof(json_str));

    log_info("%s",json_str);
    
    Message message2;
    
    app_message_initByJson(&message2,json_str,strlen((char *)json_str));
    app_message_saveBinary(&message2,result,sizeof(result));
    
    app_message_destroy(&message);
    app_message_destroy(&message2);

    assert(memcmp(data,result,9) == 0);

    return 0;
}


