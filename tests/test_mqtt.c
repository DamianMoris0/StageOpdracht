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
    	.topic = 			assembleTopic("Sensor1", TOPIC1_SUFFIX),
    	.username = 		USERNAME1,
		.password =			PASSWORD1,
	};

    TEST_ASSERT_EQUAL_MESSAGE(0, configSSL(&testClient1.sslOptions, &testClient1.connectOptions, testClient1.username, testClient1.password), "Client couldnt be created");
}

/* This test fails deliberately for demonstration purposes */
void test_connectBroker(void)
{
    MQTTClient_SSLOptions testSslOpts1 = MQTTClient_SSLOptions_initializer;
	MQTTClient_connectOptions testConnOpts1 = MQTTClient_connectOptions_initializer;
    struct Client testClient1 = {
    	.sslOptions = 		testSslOpts1,
    	.connectOptions = 	testConnOpts1,
    	.address = 			ADDRESS1,
    	.id = 				CLIENTID1,
    	.topic = 			assembleTopic("Sensor1", TOPIC1_SUFFIX),
    	.username = 		USERNAME1,
		.password =			PASSWORD1,
	};

	MQTTClient_create(&testClient1.handle, testClient1.address, testClient1.id, MQTTCLIENT_PERSISTENCE_NONE, NULL); 	// Create the MQTT client
    MQTTClient_setCallbacks(testClient1.handle, NULL, connlost, msgarrvd, delivered); 							// Set the MQTT client callback functions
    configSSL(&testClient1.sslOptions, &testClient1.connectOptions, testClient1.username, testClient1.password); 			// Configure SSL options

    TEST_ASSERT_EQUAL_MESSAGE(1, connectBroker(testClient1.handle, &testClient1.connectOptions), "Connection with broker could not be established");   // To make the test not fail change the assert value to 0 since this case should return 0
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
    	.topic = 			assembleTopic("Sensor1", TOPIC1_SUFFIX),
    	.username = 		USERNAME1,
		.password =			PASSWORD1,
	};
    char* jsonPayloadMessage = "{}";

    TEST_ASSERT_EQUAL_MESSAGE(0, publishMessage(testClient1.handle, testClient1.topic, jsonPayloadMessage, testClient1.id), "Message could not be published on topic"); 
}

void test_subscribeTopic(void)
{
	MQTTClient_SSLOptions testSslOpts1 = MQTTClient_SSLOptions_initializer;
	MQTTClient_connectOptions testConnOpts1 = MQTTClient_connectOptions_initializer;
    struct Client testClient1 = {
    	.sslOptions = 		testSslOpts1,
    	.connectOptions = 	testConnOpts1,
    	.address = 			ADDRESS1,
    	.id = 				CLIENTID1,
    	.topic = 			assembleTopic("Sensor1", TOPIC1_SUFFIX),
    	.username = 		USERNAME1,
		.password =			PASSWORD1,
	};

	TEST_ASSERT_EQUAL_MESSAGE(0, subscribeTopic(testClient1.handle, "Sensor1/"), "Could not subscribe to topic 'Sensor1/'");
}

void test_parsePayload_Get(void)
{
	/* Initialise test payloads for get function */
	char* testPayloadGet1 = "get/temp";
	char* testPayloadGet2 = "get/pres";
	char* testPayloadGet3 = "get/dfbbsvdv";
	char* testPayloadGet4 = "get/";

	/* Parse initialised test payloads for get function */
	struct ParsedPayload testParsedPayloadGet1 = parsePayload(testPayloadGet1);
	struct ParsedPayload testParsedPayloadGet2 = parsePayload(testPayloadGet2);
	struct ParsedPayload testParsedPayloadGet3 = parsePayload(testPayloadGet3);
	struct ParsedPayload testParsedPayloadGet4 = parsePayload(testPayloadGet4);

	/* Test if payload gets correctly parsed into <function> & <value> */
	TEST_ASSERT_EQUAL_STRING_MESSAGE("get", testParsedPayloadGet1.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("temp", testParsedPayloadGet1.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("get", testParsedPayloadGet2.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("pres", testParsedPayloadGet2.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("get", testParsedPayloadGet3.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("dfbbsvdv", testParsedPayloadGet3.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("get", testParsedPayloadGet4.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("", testParsedPayloadGet4.value, "Could not parse the payload value");
}

void test_parsePayload_Interval(void)
{
	/* Initialise test payloads for interval function */
	char* testPayloadInterval1 = "interval/1000000";
	char* testPayloadInterval2 = "interval/100000000000000";
	char* testPayloadInterval3 = "interval/1";
	char* testPayloadInterval4 = "interval/0";
	char* testPayloadInterval5 = "interval/-1";
	char* testPayloadInterval6 = "interval/52.365";
	char* testPayloadInterval7 = "interval/fzezgrh";
	char* testPayloadInterval8 = "interval/ ";
	char* testPayloadInterval9 = "interval/";

	/* Parse initialised test payloads for interval function */
	struct ParsedPayload testParsedPayloadInterval1 = parsePayload(testPayloadInterval1);
	struct ParsedPayload testParsedPayloadInterval2 = parsePayload(testPayloadInterval2);
	struct ParsedPayload testParsedPayloadInterval3 = parsePayload(testPayloadInterval3);
	struct ParsedPayload testParsedPayloadInterval4 = parsePayload(testPayloadInterval4);
	struct ParsedPayload testParsedPayloadInterval5 = parsePayload(testPayloadInterval5);
	struct ParsedPayload testParsedPayloadInterval6 = parsePayload(testPayloadInterval6);
	struct ParsedPayload testParsedPayloadInterval7 = parsePayload(testPayloadInterval7);
	struct ParsedPayload testParsedPayloadInterval8 = parsePayload(testPayloadInterval8);
	struct ParsedPayload testParsedPayloadInterval9 = parsePayload(testPayloadInterval9);

	/* Test if payload gets correctly parsed into <function> & <value> */
	TEST_ASSERT_EQUAL_STRING_MESSAGE("interval", testParsedPayloadInterval1.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("1000000", testParsedPayloadInterval1.value, "Could not parse the payload value");
	
	TEST_ASSERT_EQUAL_STRING_MESSAGE("interval", testParsedPayloadInterval2.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("100000000000000", testParsedPayloadInterval2.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("interval", testParsedPayloadInterval3.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("1", testParsedPayloadInterval3.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("interval", testParsedPayloadInterval4.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("0", testParsedPayloadInterval4.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("interval", testParsedPayloadInterval5.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("-1", testParsedPayloadInterval5.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("interval", testParsedPayloadInterval6.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("52.365", testParsedPayloadInterval6.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("interval", testParsedPayloadInterval7.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("fzezgrh", testParsedPayloadInterval7.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("interval", testParsedPayloadInterval8.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE(" ", testParsedPayloadInterval8.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("interval", testParsedPayloadInterval9.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("", testParsedPayloadInterval9.value, "Could not parse the payload value");
}

void test_parsePayload_Anomaly(void)
{
	/* Initialise test payloads if an unexpected value is inputted */
	char* testPayloadAnomaly1 = "zgrrdh/sfgr";
	char* testPayloadAnomaly2 = "htrh/esge/egsg/sgy/";
	char* testPayloadAnomaly3 = "sffbfb";
	char* testPayloadAnomaly4 = "/";
	char* testPayloadAnomaly5 = "//////";
	char* testPayloadAnomaly6 = " ";
	char* testPayloadAnomaly7 = "";

	/* Parse initialised test payloads for unexpected values */
	struct ParsedPayload testParsedPayloadAnomaly1 = parsePayload(testPayloadAnomaly1);
	struct ParsedPayload testParsedPayloadAnomaly2 = parsePayload(testPayloadAnomaly2);
	struct ParsedPayload testParsedPayloadAnomaly3 = parsePayload(testPayloadAnomaly3);
	struct ParsedPayload testParsedPayloadAnomaly4 = parsePayload(testPayloadAnomaly4);
	struct ParsedPayload testParsedPayloadAnomaly5 = parsePayload(testPayloadAnomaly5);
	struct ParsedPayload testParsedPayloadAnomaly6 = parsePayload(testPayloadAnomaly6);
	struct ParsedPayload testParsedPayloadAnomaly7 = parsePayload(testPayloadAnomaly7);

	/* Test what happens if payload receives anomalies */
	TEST_ASSERT_EQUAL_STRING_MESSAGE("zgrrdh", testParsedPayloadAnomaly1.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("sfgr", testParsedPayloadAnomaly1.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("htrh", testParsedPayloadAnomaly2.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("esge", testParsedPayloadAnomaly2.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("sffbfb", testParsedPayloadAnomaly3.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("", testParsedPayloadAnomaly3.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("", testParsedPayloadAnomaly4.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("", testParsedPayloadAnomaly4.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("", testParsedPayloadAnomaly5.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("", testParsedPayloadAnomaly5.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE(" ", testParsedPayloadAnomaly6.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("", testParsedPayloadAnomaly6.value, "Could not parse the payload value");

	TEST_ASSERT_EQUAL_STRING_MESSAGE("", testParsedPayloadAnomaly7.function, "Could not parse the payload function");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("", testParsedPayloadAnomaly7.value, "Could not parse the payload value");
}

void test_executeSubscribeFunction(void)
{
	/* TEST CODE */
}

void test_assembleTopic(void)
{
	/* TEST CODE */
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_configSSL);
    RUN_TEST(test_connectBroker);
    RUN_TEST(test_publishMessage);
	RUN_TEST(test_subscribeTopic); // add to testrail
	RUN_TEST(test_parsePayload_Get); // add to testrail
	RUN_TEST(test_parsePayload_Interval); // add to testrail
	RUN_TEST(test_parsePayload_Anomaly); // add to testrail
	//RUN_TEST(test_executeSubscribeFunction);
	//RUN_TEST(test_assembleTopic);
	return UNITY_END();
}
