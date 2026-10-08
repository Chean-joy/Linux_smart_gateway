#include "app_message.h"

#include <string.h>
#include <stdlib.h>

// binary -> hex
static char* bin_to_str(unsigned char *binary,int len)
{
    char *hex_str = malloc(len *2 + 1);
    if(!hex_str)
    {
        log_warn("Not enough memroy");
        return NULL;
    }

    for(int i = 0; i < len;i++)
    {
        sprintf(hex_str + i* 2,"%02X",binary[i]);
    }
    hex_str[len * 2] = '\0';
    return hex_str;

    //you must free the point after complate      free()
}

static int str_to_bin(char * hex_str,unsigned char *binary,int len)
{
    if(strlen(hex_str)%2 != 0)
    {
        log_warn("Hex string is not valid!!!\n");
        return -1;
    }
    if(len < (int)(strlen(hex_str)/2))
    {
        log_warn("Hex string is not vaild!!!\n");
        return -1;
    }
    for (int i = 0; i < len; i++)
    {
        if((hex_str[i*2] <= '9') && (hex_str[i*2] >= '0'))
        {
            binary[i] = hex_str[i*2] - '0';
        }else if((hex_str[i*2] <= 'f') && (hex_str[i*2] >= 'a'))
        {
            binary[i] = hex_str[i*2] - 'a' + 10;
        }
        else if((hex_str[i*2] <= 'F') && (hex_str[i*2] >= 'A'))
        {
            binary[i] = hex_str[i*2] - 'A' + 10;
        }

        binary[i] <<= 4;

        if((hex_str[i*2 + 1] <= '9') && (hex_str[i*2 + 1] >= '0'))
        {
            binary[i] |= (hex_str[i*2 + 1] - '0');
        }else if((hex_str[i*2 + 1] <= 'f') && (hex_str[i*2 + 1] >= 'a'))
        {
            binary[i] |= (hex_str[i*2 + 1] - 'a' + 10);
        }
        else if((hex_str[i*2 + 1] <= 'F') && (hex_str[i*2 + 1] >= 'A'))
        {
            binary[i] |= (hex_str[i*2 + 1] - 'A' + 10);
        }

    }
    return len;
}




/***
 * {
 * "connection_type":2,
 * "id":"EA88",
 * "data":"AABBCCDDEEFF"
 * }
 */
int app_message_initByJson(Message *message, void *json_string,int len)
{
    cJSON *json_object = cJSON_ParseWithLength(json_string,len);

    cJSON *Connection_type = cJSON_GetObjectItem(json_object,"connection_type");  
    message->connection_type = Connection_type->valueint;

    //get read id len and data len
    cJSON *id = cJSON_GetObjectItem(json_object,"id");
    if(strlen(id->valuestring)%2 != 0)
    {
        log_warn("Message is not valid");
        return -1;
    } 
    message->id_len = strlen(id->valuestring)/2;

    cJSON* data = cJSON_GetObjectItem(json_object,"data");

    if(strlen(data->valuestring)%2 != 0)
    {
        log_warn("Message is not vaild");
        return -1;
    }

    message->msg_len = strlen(data->valuestring)/2;
    message->payload = malloc(message->id_len + message->msg_len);

    if(!message->payload)
    {
        log_warn("Not Enough for message!!!\n");
        return -1;
    }

    if(str_to_bin(id->valuestring,message->payload,message->id_len) < 0)
    {
        log_warn("Convertion faild");
        free(message->payload);
        return -1;
    }
    if(str_to_bin(data->valuestring,message->payload + message->id_len,
    message->msg_len)<0)
    {
        log_warn("Convertion faild");
        free(message->payload);
        return -1;
    }
    //BUG:
    // log_info("id:%s,data:%s",id->valuestring,data->valuestring);

    // for (int i = 0; i < (message->id_len + message->msg_len); i++)
    // {
    //     log_info("%d : %02x",i,message->payload[i]);
    // }

    cJSON_Delete(json_object);

    return 0;
}

int app_message_saveJson(Message *message, void *ptr, int len)
{
    cJSON *json_object = cJSON_CreateObject();

    cJSON_AddNumberToObject(json_object,"connection_type",message->connection_type);
    cJSON_AddStringToObject(json_object,"id",bin_to_str(message->payload,message->id_len));
    cJSON_AddStringToObject(json_object,"data",bin_to_str(message->payload+message->id_len,message->msg_len));

    char *str = cJSON_PrintUnformatted(json_object);
    if(len < (int)(strlen(str) + 1))
    {
        log_warn("Buffer not enough for message");
        return -1;
    }
    strcpy(ptr,str);

    cJSON_free(str);
    cJSON_Delete(json_object);
    return 0;
}

int app_message_initByBinary(Message *message, void *ptr, int len)
{
    memset(message,0,sizeof(Message));

    memcpy(&message->connection_type,ptr,1);
    memcpy(&message->id_len,ptr+1,1);
    memcpy(&message->msg_len,ptr+2,1);

    if(len != (message->id_len + message->msg_len + 3))
    {
        log_warn("Message is not valid!!!");
        return -1;
    }

    message->payload = malloc(message->id_len + message->msg_len);
    
    if(!message->payload)
    {
        log_warn("Not Enought for message");
        return -1;
    }
    memcpy(message->payload,ptr+3,message->id_len+message->msg_len);

    return 0;
}

int app_message_saveBinary(Message *message, void *ptr, int len)
{
    int total_len = message->id_len + message->msg_len + 3;

    if(len < total_len)
    {
        log_warn("Buffer not enought for message");
        return -1;
    }
    memcpy(ptr,&message->connection_type,1);
    memcpy(ptr + 1,&message->id_len,1);
    memcpy(ptr + 2,&message->msg_len,1);
    memcpy(ptr + 3,message->payload,message->id_len + message->msg_len);

    //debug:
    // log_info("%02x",*((unsigned char*)ptr));
    // log_info("%02x",*((unsigned char*)(ptr + 1)));

    return total_len;
}

void app_message_destroy(Message *message)
{
    if(message->payload)
    {
        free(message->payload);
        message->payload = NULL;
    }
}
