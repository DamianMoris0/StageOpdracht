#include "../unity/unity.h"
#include "MQTTClient.h"
#include "../lib/mqtt.h"

void setUp(void) {}
void tearDown(void) {}

void test_configSSL(void)
{
    MQTTClient_SSLOptions testSslOpts1 = MQTTClient_SSLOptions_initializer;
	MQTTClient_connectOptions testConnOpts1 = MQTTClient_connectOptions_initializer;
    struct Client testClient1 = {
    	.sslOptions = 		testSslOpts1,
    	.connectOptions = 	testConnOpts1,
    	.address = 			ADDRESS1,
    	.id = 				CLIENTID1,
    	.topic = 			TOPIC1,
    	.username = 		USERNAME1,
		.password =			PASSWORD1,
	};

    TEST_ASSERT_EQUAL_MESSAGE(0, configSSL(&testClient1.sslOptions, &testClient1.connectOptions, testClient1.username, testClient1.password), "Succesfully created client");
}

void test_connectBroker(void)
{
    MQTTClient_SSLOptions testSslOpts1 = MQTTClient_SSLOptions_initializer;
	MQTTClient_connectOptions testConnOpts1 = MQTTClient_connectOptions_initializer;
    struct Client testClient1 = {
    	.sslOptions = 		testSslOpts1,
    	.connectOptions = 	testConnOpts1,
    	.address = 			ADDRESS1,
    	.id = 				CLIENTID1,
    	.topic = 			TOPIC1,
    	.username = 		USERNAME1,
	};

    TEST_ASSERT_EQUAL_MESSAGE(0, connectBroker(testClient1.handle, &testClient1.connectOptions), "Succesfully connected to broker"); 
}

void test_publishMessage(void)
{
    MQTTClient_SSLOptions testSslOpts1 = MQTTClient_SSLOptions_initializer;
	MQTTClient_connectOptions testConnOpts1 = MQTTClient_connectOptions_initializer;
    struct Client testClient1 = {
    	.sslOptions = 		testSslOpts1,
    	.connectOptions = 	testConnOpts1,
    	.address = 			ADDRESS1,
    	.id = 				CLIENTID1,
    	.topic = 			TOPIC1,
    	.username = 		USERNAME1,
	};
    char* jsonPayloadMessage = "{}";

    TEST_ASSERT_EQUAL_MESSAGE(0, publishMessage(testClient1.handle, testClient1.topic, jsonPayloadMessage, testClient1.id), "Succesfully published message on topic"); 
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_configSSL);
    RUN_TEST(test_connectBroker);
    RUN_TEST(test_publishMessage);
	return UNITY_END();
}
