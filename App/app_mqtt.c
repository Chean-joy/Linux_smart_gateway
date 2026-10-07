#include "app_mqtt.h"
#include "Thirdparty/LOG/log.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static MQTTClient client; 
static MQTTClient_connectOptions conn_opts= MQTTClient_connectOptions_initializer;
static MQTTClient_deliveryToken deliveredtoken;
static int (*recvCallback)(void *ptr, int len);


void app_mqtt_connectionLost(void* context, char* cause)
{
    assert(context == NULL);
    log_fatal("MQTT Connect lost because of %s,closing...",cause);
    exit(EXIT_FAILURE);
}

int app_mqtt_messageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* message)
{
    int result = 0;
    assert(context == NULL);
    log_trace("Message from topic%.*s arrived, content %.*s",
    topicLen, topicName, message->payloadlen, (char *)message->payload);
    result = (recvCallback(message->payload, message->payloadlen) == 0 ? 1 : 0);
    
    return result;
}

void app_mqtt_deliveryComplete(void* context, MQTTClient_deliveryToken dt)
{
    assert(context == NULL);
    log_trace("Message with token value %d delivery confirmed",
    dt);
    deliveredtoken = dt;
}


int app_mqtt_init()
{
    int res;
    res = MQTTClient_create(&client,MQTT_SERVER_URL,MQTT_CLIENT_ID,
        MQTTCLIENT_PERSISTENCE_NONE,NULL);
    if(res != MQTTCLIENT_SUCCESS)
    {
        log_warn("MQTTClient_create failed,rc %d\n",res);
        goto CREATE_FAIL;
    }

    if(MQTTClient_setCallbacks(client,NULL,app_mqtt_connectionLost,
        app_mqtt_messageArrived,app_mqtt_deliveryComplete) != MQTTCLIENT_SUCCESS)
    {
        log_warn("MQTT set callback fail");
        goto CONNECT_FAIL;
    }

     // 连接MQTT服务器
    conn_opts.connectTimeout = 1000;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS)
    {
        log_warn("MQTT connect fail");
        goto CONNECT_FAIL;
    }


    if(MQTTClient_subscribe(client,MQTT_TOPIC,0) != MQTTCLIENT_SUCCESS)
    {
        log_warn("MQTT subscribe fail");
        goto SUBSCRIBE;
    }

    return 0;

SUBSCRIBE:
    MQTTClient_disconnect(client,1000);
CONNECT_FAIL:
    MQTTClient_destroy(&client);
CREATE_FAIL:
    app_mqtt_close();
    return -1;
}

void app_mqtt_close()
{
    MQTTClient_disconnect(client,1000);
    MQTTClient_destroy(&client);
}

void app_mqtt_registerRecvCallback(int (*recv_callback)(void *, int))
{
    recvCallback = recv_callback;
}

int app_mqtt_send(char *json_str,int len)
{
    log_trace("Message: %s,len: %d",json_str,len);
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    int result = 0;

    pubmsg.payload = json_str;
    pubmsg.payloadlen = len;
    pubmsg.qos = 0;
    pubmsg.retained = 0;
    deliveredtoken = 0;

    if(MQTTClient_publishMessage(client, MQTT_TOPIC, &pubmsg, &token) 
    != MQTTCLIENT_SUCCESS)
    {
        log_warn("Message send fail");
        result = -1;
    }
    else
    {
        result = 0;
    }
    return result;
}
