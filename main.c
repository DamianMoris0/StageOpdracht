#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "MQTTClient.h"
#include "lib/bmp180driver.h"
#include "lib/sensor.h"
#include "lib/mqtt.h"

#define MAX_FAULTY_SENSOR_READINGS 60 // If sensor data is out of bounds MAX_FAULTY_SENSOR_READINGS times, exit device execution

int main()
{
	/* Initialise variables */
	char *i2c = "/dev/i2c-1";
	int addr = 0x77;
	void *sensor = bmp180_init(addr, i2c);
	time_t rawtime;
	struct tm *timeinfo;
	FILE *sensorDataFile;
	struct SensorValues sensorValues;
	sensorValues.typeStr = getSensorTypeName(BMP180);
	sensorValues.id = "Sensor_1";
	char* jsonPayloadMessage = createJsonFromSensorData(&sensorValues);
	int loopCounter = 0;
	int loopAmount = 1;

	/* Inintialise client 1 */
	MQTTClient_SSLOptions sslOpts1 = MQTTClient_SSLOptions_initializer;
	MQTTClient_connectOptions connOpts1 = MQTTClient_connectOptions_initializer;
	struct Client client1 = {
    	.sslOptions = 		sslOpts1,
    	.connectOptions = 	connOpts1,
    	.address = 			ADDRESS1,
    	.id = 				CLIENTID1,
    	.topic = 			TOPIC1,
    	.username = 		USERNAME1,
	};

	/* Inintialise client 2 */
	MQTTClient_SSLOptions sslOpts2 = MQTTClient_SSLOptions_initializer;
	MQTTClient_connectOptions connOpts2 = MQTTClient_connectOptions_initializer;
	struct Client client2 = {
    	.sslOptions = 		sslOpts2,
    	.connectOptions = 	connOpts2,
    	.address = 			ADDRESS2,
    	.id = 				CLIENTID2,
    	.topic = 			TOPIC2,
    	.username = 		USERNAME2,
	};

	/* MQTT client x broker config & connection*/
	/* Client 1 */
    MQTTClient_create(&client1.handle, client1.address, client1.id, MQTTCLIENT_PERSISTENCE_NONE, NULL); 	// Create the MQTT client
    MQTTClient_setCallbacks(client1.handle, NULL, connlost, msgarrvd, delivered); 							// Set the MQTT client callback functions
    configSSL(&client1.sslOptions, &client1.connectOptions, client1.username); 								// Configure SSL options
    connectBroker(client1.handle, &client1.connectOptions); 												// Connect to the MQTT broker

	/* Client 2 */
    MQTTClient_create(&client2.handle, client2.address, client2.id, MQTTCLIENT_PERSISTENCE_NONE, NULL); 	// Create the MQTT client
    MQTTClient_setCallbacks(client2.handle, NULL, connlost, msgarrvd, delivered); 							// Set the MQTT client callback functions
    configSSL(&client2.sslOptions, &client2.connectOptions, client2.username); 								// Configure SSL options
    connectBroker(client2.handle, &client2.connectOptions); 												// Connect to the MQTT broker

	/* Initialise sensor */
	initSensor(&sensor, sensorValues.typeID);

	/* Open data.json file */
	sensorDataFile = fopen("/var/data/data.json", "w");
	if (sensorDataFile == NULL) {
		printf("Unable to open file\n");
		return 1;
	}

	/* Set amount of loops needed for file writing */
	if (SAMPLE_TIME > SEND_JSON_DATA_FILE_INTERVAL) {
		loopAmount = 1;
	} else {
		loopAmount = SEND_JSON_DATA_FILE_INTERVAL / SAMPLE_TIME; // Calculate amount of loops needed for specified SEND_JSON_DATA_FILE_INTERVAL per SAMPLE_TIME
	}

	/* Read sensor data and publish on mqtt topic + write to file if specified */
	for (;;) {
		for (int i = 0; i < loopAmount; i++) {
			/* Convert pressure data into hPa + get one decimal place accuracy on values */
			sensorValues.temperature = round(bmp180_temperature(sensor) * 100) / 100.0;
			sensorValues.pressure = round(bmp180_pressure(sensor) / 10) / 10;

			/* Check data bounds of sensor data values */
			if (checkDataBounds(sensorValues.temperature, sensorValues.pressure)) {
				printf("Sensor data out of bounds\n");
				i--; // Decrement i and continue to read again, so json file will be the same size
				loopCounter++;
				if (loopCounter >= MAX_FAULTY_SENSOR_READINGS) {
					printf("Sensor possibly broken, %d readins were out of bounds\n", MAX_FAULTY_SENSOR_READINGS);
					break;
				}
				continue;
			}
			loopCounter = 0;

			/* Get raw time and convert to readable time + remove '\n' from last char in array */
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			sensorValues.time = asctime(timeinfo);
			sensorValues.time[strlen(sensorValues.time) - 1] = '\0';

			/* Publish MQTT message on topic + write sensor data to file for potential future use, depending on if corresponding flags are turned on/off */
			jsonPayloadMessage = createJsonFromSensorData(&sensorValues);
			if (publish_json_on_mqtt_topic_flag) {
				publishMessage(client1.handle, client1.topic, jsonPayloadMessage, client1.id); // Client 1
				publishMessage(client2.handle, client2.topic, jsonPayloadMessage, client2.id); // Client 2
			}
			if (write_to_json_file_flag) {
				writeSensorDataToFile(&sensorDataFile, jsonPayloadMessage);
			}
			usleep(SAMPLE_TIME); // Time in µs
		}
		if (write_to_json_file_flag || loopCounter >= MAX_FAULTY_SENSOR_READINGS) {
			break; // If data is written to file break loop to be able to access data.json file, can be changed in the future for other purposes
		}
	}

	/* Free allocated memory for jsonPayloadMessage */
	free(jsonPayloadMessage);

	/* Close file and bmp180 */
	fclose(sensorDataFile);
	bmp180_close(sensor);

	/* Disconnect MQTT connection */
    MQTTClient_disconnect(client1.handle, 10000);
	MQTTClient_disconnect(client2.handle, 10000);
    MQTTClient_destroy(&client1.handle);
	MQTTClient_destroy(&client2.handle);

	return 0;
}
