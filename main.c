#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
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
    char *payloadMessage = "{\"sensorID\":\"Sensor_1\",\"timestamp\":\"Thu Oct 24 16:59:29 2024\",\"temperature\":18.3,\"pressure\":1006.0}";

	/* MQTT client x broker config & connection*/
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL); 	// Create the MQTT client
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered); 				// Set the MQTT client callback functions
    configSSL(&sslOptions, &connectOptions); 											// Configure SSL options
    connectBroker(client, &connectOptions); 											// Connect to the MQTT broker
    publishMessage(client, payloadMessage); 											// Publish a message

	/* Initialise sensor */
	initSensor(&sensor, sensorValues.type);

	/* Open data.json file */
	sensorDataFile = fopen("/var/data/data.json", "w");
	if (sensorDataFile == NULL) {
		printf("Unable to open file\n");
		return 1;
	}

	/* Read sensor data and write to file */
	for (int i = 0; i < 10; i++) {											// This will become an infinite loop when database structure is finished
		sensorValues.temperature = bmp180_temperature(sensor);
		sensorValues.pressure = bmp180_pressure(sensor) / 100; 				// Divide by 100 to get hPa
		checkDataBounds(sensorValues.temperature, sensorValues.pressure);	// If temperature or pressure are out of bounds return 1

		/* Get raw time and convert to readable time and remove '\n' from last char in array */
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		sensorValues.time = asctime(timeinfo);
		sensorValues.time[strlen(sensorValues.time) - 1] = '\0';

		writeSensorDataToFile(&sensorDataFile, &sensorValues);
		usleep(100000); 													// Time in µs
	}

	/* Close file and bmp180 */
	fclose(sensorDataFile);
	bmp180_close(sensor);

	/* Disconnect MQTT connection */
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

	return 0;
}
