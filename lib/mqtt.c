#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include "mqtt.h"

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int configSSL(MQTTClient_SSLOptions *ssl_opts, MQTTClient_connectOptions *conn_opts, char* username)
{
    /* SSL/TLS options */
    //ssl_opts->trustStore = NULL;             // CA certificate (if available, otherwise not required)
    //ssl_opts->keyStore = CLIENT_CERT_PATH;   // Client certificate
    //ssl_opts->privateKey = CLIENT_KEY_PATH;  // Client private key
    //ssl_opts->enableServerCertAuth = 0;      // If CA certificate available set to 1, otherwise 0
    //ssl_opts->sslVersion = MQTT_SSL_VERSION_TLS_1_2;

    /* Connection options */
    conn_opts->keepAliveInterval = 20;
    conn_opts->cleansession = 1;
    //conn_opts->ssl = ssl_opts;
    conn_opts->username = username;

    return 0;
}

int connectBroker(MQTTClient handle, MQTTClient_connectOptions *conn_opts)
{
    int returnCode;
    if ((returnCode = MQTTClient_connect(handle, conn_opts)) == MQTTCLIENT_SUCCESS) {
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
