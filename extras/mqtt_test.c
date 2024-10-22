#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

#define ADDRESS     "ssl://IotHubStage.azure-devices.net:8883"  // Broker address with SSL
#define CLIENTID    "RaspberryPi"  // Client ID for your MQTT client
#define TOPIC       "devices/RaspberryPi/messages/events/"  // Change to your topic
#define PAYLOAD     "Hello MQTT with SSL"
#define QOS         1
#define TIMEOUT     10000L

// Paths to your certificates
#define CLIENT_CERT_PATH  "../../Certificates/cert.pem"
#define CLIENT_KEY_PATH   "../../Certificates/key_nopass.pem"
//#define CA_CERT_PATH      "../../Certificates/azure-iot-root-ca.pem" // Optional, but recommended

void delivered(void *context, MQTTClient_deliveryToken dt) {
    printf("Message with token value %d delivery confirmed\n", dt);
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause) {
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
    int rc;
    
    // Create the client
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    // Set the callback functions
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    
    // SSL/TLS options
    ssl_opts.trustStore = NULL;   // CA certificate (if available)
    ssl_opts.keyStore = CLIENT_CERT_PATH;  // Client certificate
    ssl_opts.privateKey = CLIENT_KEY_PATH; // Client private key
    ssl_opts.sslVersion = MQTT_SSL_VERSION_TLS_1_2;
    ssl_opts.enableServerCertAuth = 0;     // Enable server certificate verification

    // Connection options
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.ssl = &ssl_opts;
    conn_opts.username = "IotHubStage.azure-devices.net/RaspberryPi/?api-version=2021-04-12"; // Replace with your device ID

    // Connect to the broker
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        
        // Add general error handling
        switch (rc) {
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
                printf("Error: Unknown error code %d\n", rc);
                break;
        }
        exit(EXIT_FAILURE);
    }

    // Publish a message
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen = (int)strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    printf("Waiting for publication of %s on topic %s for client with ClientID: %s\n", PAYLOAD, TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);

    // Disconnect
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    return rc;
}
