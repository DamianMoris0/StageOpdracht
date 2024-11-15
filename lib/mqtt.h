#include "MQTTClient.h"

#define ADDRESS1     "mqtt-dashboard.com"  // Broker address with SSL "mqtt-dashboard.com"
#define ADDRESS2     "ssl://WerkpleklerenBrightest.azure-devices.net:8883" 
#define CLIENTID1    "raspberry-testclient-stage"                               // Client ID for your MQTT client
#define CLIENTID2    "RPI"
#define TOPIC1       "testtopic/"      // Change to your topic "testtopic/"
#define TOPIC2       "devices/RPI/messages/events/"              // Change to your topic "testtopic/"
#define USERNAME1    "stage" //"stage"
#define USERNAME2    "WerkpleklerenBrightest.azure-devices.net/RPI/?api-version=2021-04-12"
#define QOS          1
#define TIMEOUT      10000L

/* Paths to your certificates */
#define CLIENT_CERT_PATH  "../Certificates/cert.pem"
#define CLIENT_KEY_PATH   "../Certificates/key_nopass.pem"

struct Client {
    MQTTClient handle;
    MQTTClient_SSLOptions sslOptions;
    MQTTClient_connectOptions connectOptions;
    char* address;
    char* id;
    char* topic;
    char* username;
};

void delivered(void *context, MQTTClient_deliveryToken dt);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connlost(void *context, char *cause);
int configSSL(MQTTClient_SSLOptions *ssl_opts, MQTTClient_connectOptions *conn_opts, char* username);
int connectBroker(MQTTClient handle, MQTTClient_connectOptions *conn_opts);
int publishMessage(MQTTClient handle, char* topic, char* payload, char* clientid);