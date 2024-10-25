#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "MQTTClient.h"
#include "lib/bmp180driver.h"
#include "lib/sensor.h"
#include "lib/mqtt.h"

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
	sensorValues.type = BMP180;
	sensorValues.id = "Sensor_1";

	/* MQTT variables */
	MQTTClient client;
    MQTTClient_SSLOptions sslOptions = MQTTClient_SSLOptions_initializer;
    MQTTClient_connectOptions connectOptions = MQTTClient_connectOptions_initializer;
    char* jsonPayloadMessage = createJsonFromSensorData(&sensorValues);

	/* MQTT client x broker config & connection*/
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL); 	// Create the MQTT client
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered); 				// Set the MQTT client callback functions
    configSSL(&sslOptions, &connectOptions); 											// Configure SSL options
    connectBroker(client, &connectOptions); 											// Connect to the MQTT broker

	/* Initialise sensor */
	initSensor(&sensor, sensorValues.type);

	/* Open data.json file */
	sensorDataFile = fopen("/var/data/data.json", "w");
	if (sensorDataFile == NULL) {
		printf("Unable to open file\n");
		return 1;
	}

	/* Read sensor data and write to file */
	int loopAmount = SEND_DATA_INTERVAL / SAMPLE_TIME; // Specifies the amount of loops needed to send full json data file every SEND_DATA_INTERVAL seconds, with data samples every SAMPLE_TIME seconds
	for (int i = 0; i < loopAmount; i++) {
		/* Convert pressure data into hPa + get one decimal place accuracy on values */
		sensorValues.temperature = round(bmp180_temperature(sensor) * 100) / 100.0;
		sensorValues.pressure = round(bmp180_pressure(sensor) / 10) / 10;

		/* Check data bounds of sensor data values */
		if (checkDataBounds(sensorValues.temperature, sensorValues.pressure)) {
			printf("Sensor data out of bounds\n");
			i--; // Decrement i and continue to read again, so json file will be the same size
			continue;
		}

		/* Get raw time and convert to readable time + remove '\n' from last char in array */
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		sensorValues.time = asctime(timeinfo);
		sensorValues.time[strlen(sensorValues.time) - 1] = '\0';

		/* Publish MQTT message on topic + write sensor data to file for potential future use */
		jsonPayloadMessage = createJsonFromSensorData(&sensorValues);
		publishMessage(client, jsonPayloadMessage);
		writeSensorDataToFile(&sensorDataFile, jsonPayloadMessage);
		usleep(SAMPLE_TIME); // Time in µs
	}

	/* Close file and bmp180 */
	fclose(sensorDataFile);
	bmp180_close(sensor);

	/* Disconnect MQTT connection */
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

	return 0;
}
