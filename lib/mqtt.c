#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include "mqtt.h"

char receivedPayload[128];
char message_received_flag;

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
    //char* function = NULL;
    //char* value = NULL;

    pPayload.function = (char*)malloc(1);
    pPayload.value = (char*)malloc(1);

    printf("%d\n", strlen(payload));
    for (int i = 0; i < strlen(payload); i++) {
        if (payload[i] != '/') {
            pPayload.function = realloc(pPayload.function, strlen(pPayload.function) + 1);
            pPayload.function[i] = payload[i];
            pPayload.function[i + 1] = '\0';
            //printf("\npayload[%d] != '/'\nfunction[%d] = %c\nfunction = %s\n\n", i, i, pPayload.function[i], pPayload.function);
            //strcpy(function, (char*)payload[i]);
        }
        else {
            printf("Function is: %s\n", pPayload.function);
            break;
            //return function;
        }
    }

    for (int i = 0; i < strlen(payload) - strlen(pPayload.function) - 1; i++) {
        //pPayload.value[i - strlen(pPayload.function) - 1] = payload[i] != '\0' ? payload[i] : '\0';
        if (payload[i + strlen(pPayload.function) + 1] != '\0') {
            pPayload.value = realloc(pPayload.value, strlen(pPayload.value) + 1);
            pPayload.value[i] = payload[i + strlen(pPayload.function) + 1];
            pPayload.value[i + 1] = '\0';
            printf("\n%c%c %d\n", pPayload.value[i], payload[i + strlen(pPayload.function) + 1], strlen(pPayload.value));
            //printf("\npayload[%d] != '\\0'\nvalue[%d] = %c\nvalue = %s\n\n", i, i - strlen(pPayload.function) - 1, pPayload.value[i - strlen(pPayload.function) - 1], pPayload.value);
        }
        else {
            printf("Value is: %s\n", pPayload.value);
            break;
        }
    }

    printf("Succesfully parsed message, %s %s\n", pPayload.function, pPayload.value);

    //free(pPayload.function);
    //free(pPayload.value);

    return pPayload;
}