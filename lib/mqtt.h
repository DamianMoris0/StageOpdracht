#include "MQTTClient.h"

#define ADDRESS     "ssl://IotHubStage.azure-devices.net:8883"  // Broker address with SSL
#define CLIENTID    "RaspberryPi"                               // Client ID for your MQTT client
#define TOPIC       "devices/RaspberryPi/messages/events/"      // Change to your topic
#define QOS         1
#define TIMEOUT     10000L

/* Paths to your certificates */
#define CLIENT_CERT_PATH  "../Certificates/cert.pem"
#define CLIENT_KEY_PATH   "../Certificates/key_nopass.pem"

void delivered(void *context, MQTTClient_deliveryToken dt);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connlost(void *context, char *cause);
void configSSL(MQTTClient_SSLOptions *ssl_opts, MQTTClient_connectOptions *conn_opts);
int connectBroker(MQTTClient handle, MQTTClient_connectOptions *conn_opts);
int publishMessage(MQTTClient handle, char* payload);