#include "bmp180driver.h"
#include "sensor.h"
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
		return 1;
		/* Default to BMP180 sensor */
		//bmp180_eprom_t eprom;
		//bmp180_dump_eprom(sens, &eprom);
		//bmp180_set_oss(sens, 1);
	}

    return 0;
}

int writeSensorDataToFile(FILE **dataFile, struct SensorValues *sensVals)
{
	// Write sensor values to file in json format
	fprintf(*dataFile, "{\"sensorID\":\"%s\",", sensVals->id);
	fprintf(*dataFile, "\"timestamp\":\"%s\",", sensVals->time);
	fprintf(*dataFile, "\"temperature\":%.1f,", sensVals->temperature);
	fprintf(*dataFile, "\"pressure\":%.1f}\n", sensVals->pressure);

	return 0;
}

int checkDataBounds(float temp, float pres)
{
	if (temp <= -50 || temp >= 50) {
		return 1;
	}

	if (pres <= 800 || pres >= 1200) {
		return 1;
	}

	return 0;
}