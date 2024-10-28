#include "bmp180driver.h"
#include "sensor.h"
#include "cJSON.h"
#include <stdio.h>
#include <time.h>

int initSensor(void *sens, int type)
{
	if (sens == NULL) {
		printf("Unable to detect sensor\n");
		return 1;
	}

	if (type == BMP180) {
		bmp180_eprom_t eprom;
		bmp180_dump_eprom(sens, &eprom);
		bmp180_set_oss(sens, 1);
	}
	else {
		printf("Sensor type not found");
		return 1;
		/* Default to BMP180 sensor */
		//bmp180_eprom_t eprom;
		//bmp180_dump_eprom(sens, &eprom);
		//bmp180_set_oss(sens, 1);
	}

    return 0;
}

char* createJsonFromSensorData(struct SensorValues *sensVals)
{
    /* Create a json object */
    cJSON *json = cJSON_CreateObject();
    
    /* Add data from struct to JSON */
    cJSON_AddStringToObject(json, "sensorID", 	 sensVals->id);
	cJSON_AddStringToObject(json, "sensorType",	 sensVals->typeStr);
    cJSON_AddStringToObject(json, "timestamp", 	 sensVals->time);
    cJSON_AddNumberToObject(json, "temperature", sensVals->temperature);
    cJSON_AddNumberToObject(json, "pressure", 	 sensVals->pressure);

	/* Convert json object to string */
    char *jsonString = cJSON_Print(json);

	/* Clean json object */
    cJSON_Delete(json);

	return jsonString;
}

int writeSensorDataToFile(FILE **dataFile, char* jsonSensVals)
{
	/* Write sensor values to file in json format */
	fprintf(*dataFile, "%s", jsonSensVals);

	return 0;
}

int checkDataBounds(double temp, double pres)
{
	if (temp <= -50 || temp >= 50) {
		return 1;
	}

	if (pres <= 800 || pres >= 1200) {
		return 1;
	}

	return 0;
}

char* getSensorTypeName(int type)
{
	char* sensorTypeName = "DEFAULT_SENSOR";

	switch (type) {
	case BMP180:
		sensorTypeName = "BMP180";
		break;
	default:
		sensorTypeName = "DEFAULT_SENSOR";
		break;
	}

	return sensorTypeName;
}