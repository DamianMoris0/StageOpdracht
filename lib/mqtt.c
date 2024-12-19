#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include "mqtt.h"
#include "sensor.h"

char receivedPayload[128];
char message_received_flag;
int variableSampleTime;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    message_received_flag = 1;

    printf("Message arrived\n");
    printf("Topic: %s\n", topicName);
    printf("Message: %.*s\n", message->payloadlen, (char*)message->payload);

    strncpy(receivedPayload, (char *)message->payload, message->payloadlen);
    receivedPayload[message->payloadlen] = '\0'; // Null-terminate the string

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("Cause: %s\n", cause);
}

int configSSL(MQTTClient_SSLOptions *ssl_opts, MQTTClient_connectOptions *conn_opts, char* user, char* pass)
{
    /* SSL/TLS options */
    ssl_opts->trustStore = NULL;             // CA certificate (if available, otherwise not required)
    ssl_opts->keyStore = CLIENT_CERT_PATH;   // Client certificate
    ssl_opts->privateKey = CLIENT_KEY_PATH;  // Client private key
    ssl_opts->enableServerCertAuth = 0;      // If CA certificate available set to 1, otherwise 0
    ssl_opts->sslVersion = MQTT_SSL_VERSION_TLS_1_2;

    /* Connection options */
    conn_opts->keepAliveInterval = 20;
    conn_opts->cleansession = 1;
    conn_opts->ssl = ssl_opts;  // disable this + all ssl_opts above for use without ssl (in the docker)
    conn_opts->username = user;
    conn_opts->password = pass;

    return 0;
}

int connectBroker(MQTTClient handle, MQTTClient_connectOptions *conn_opts)
{
    int returnCode;
    if ((returnCode = MQTTClient_connect(handle, conn_opts)) == MQTTCLIENT_SUCCESS) {
        printf("Client succesfully connected to broker\n");
        return 0;
    }
    else {
        // If client fails to connect to the broker, some general error handling for debugging
        printf("Failed to connect, return code %d\n", returnCode);
        switch (returnCode) {
            case MQTTCLIENT_FAILURE:
                printf("Error: Generic failure\n");
                break;
            case MQTTCLIENT_SSL_NOT_SUPPORTED:
                printf("Error: SSL not supported\n");
                break;
            case MQTTCLIENT_BAD_PROTOCOL:
                printf("Error: Bad protocol version\n");
                break;
            default:
                printf("Error: Unknown error code %d\n", returnCode);
                break;
        }
        return returnCode;
    }
}

int publishMessage(MQTTClient handle, char* topic, char* payload, char* clientid)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = (int)strlen(payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;

    MQTTClient_publishMessage(handle, topic, &pubmsg, &token);
    printf("Waiting for publication of %s on topic %s for client with ClientID: %s\n", payload, topic, clientid);
    MQTTClient_waitForCompletion(handle, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);

    return 0;
}

int subscribeTopic(MQTTClient handle, char* topic)
{
    int returnCode;
    if ((returnCode = MQTTClient_subscribe(handle, topic, QOS)) == MQTTCLIENT_SUCCESS) {
        printf("Client succesfully subscribed on topic %s\n", topic);
		return 0;
	}
    else {
        printf("Failed to subscribe, return code %d\n", returnCode);
		MQTTClient_disconnect(handle, TIMEOUT);
		MQTTClient_destroy(&handle);
		exit(EXIT_FAILURE);
    }
	printf("Subscribed to topic: %s\n", topic);
}

/**********************************************************************************************************/
//  Payload funcitons:
//  get/ ==> followed by temp or pres to get the current temperature or pressure
//  interval/ ==> follwed by an integer value (in ms) to change the interval in which the data is sent in
/**********************************************************************************************************/
struct ParsedPayload parsePayload(char* payload)
{
    struct ParsedPayload pPayload = {
        .function = NULL,
        .value = NULL,
    };

    pPayload.function = (char*)malloc(1);
    pPayload.value = (char*)malloc(1);

    for (int i = 0; i < strlen(payload); i++) {
        if (payload[i] != '/') {
            pPayload.function = realloc(pPayload.function, strlen(pPayload.function) + 1);
            pPayload.function[i] = payload[i];
            pPayload.function[i + 1] = '\0';
        }
        else {
            break;
            //return function;
        }
    }

    for (int i = 0; i < strlen(payload) - strlen(pPayload.function) - 1; i++) {
        if (payload[i + strlen(pPayload.function) + 1] != '\0') {
            pPayload.value = realloc(pPayload.value, strlen(pPayload.value) + 1);
            pPayload.value[i] = payload[i + strlen(pPayload.function) + 1];
            pPayload.value[i + 1] = '\0';
        }
        else {
            break;
        }
    }

    printf("Succesfully parsed message, funtion = %s value = %s\n", pPayload.function, pPayload.value);

    return pPayload;
}

int executeSubscribeFunction(MQTTClient handle, char* id, struct ParsedPayload ppayload, double temp, double pres)
{
    if (!strcmp(ppayload.function, "get")) {
        if (!strcmp(ppayload.value, "temp")) {
            char bufferStrTemp[sizeof(temp)];
            sprintf(bufferStrTemp, "%.2f", temp);
            publishMessage(handle, "sensor/get/", bufferStrTemp, id);
            printf("Message published on topic sensor/get/ with payload %s\n", bufferStrTemp);
        }
        else if (!strcmp(ppayload.value, "pres")) {
            char bufferStrPres[sizeof(pres)];
            sprintf(bufferStrPres, "%.2f", pres);
            publishMessage(handle, "sensor/get/", bufferStrPres, id);
            printf("Message published on topic sensor/get/ with payload %s\n", bufferStrPres);
        }
        else {
            publishMessage(handle, "sensor/get/", "Wrong Input", id);
            printf("ERROR: function get was called but value was wrong.\n");
        }
        return 0;
    }
    else if (!strcmp(ppayload.function, "interval")) {
        /* variableSampleTime has to to be more than 0.1s and less than 5min else goes to default value */
        printf("Function 'interval' was called with payload %s\n", ppayload.value);
        if (variableSampleTime >= 100000 && variableSampleTime <= 300000000) {
            variableSampleTime = atoi(ppayload.value);
        }
        else {
            printf("Changed sample time out of bounds [100000 - 300000000], resorting to default\n");
            variableSampleTime = DEFAULT_SAMPLE_TIME;
        }
        return 0;
    }
    else {
        printf("Function wasn't able to be deciphered\n");
        return 1;
    }
}

char* assembleTopic(char* sensorId, char* suffix)
{
    char* topic;
    topic = (char*)malloc(strlen(sensorId) + strlen(suffix) + 1);
	strcpy(topic, sensorId);
	strcat(topic, suffix);
    return topic;
}