#include "bmp180driver.h"
#include "sensor.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int initSensor(void *sensor)
{
    bmp180_eprom_t eprom;
	bmp180_dump_eprom(sensor, &eprom);
	bmp180_set_oss(sensor, 1);
	if (sensor == NULL) {
		printf("Unable to detect sensor\n");
		return 1;
	}
    return 0;
}

int writeSensorDataToFile(FILE **dataFile, float *temp, float *pres, struct tm *time)
{
	// convert time to readable time and remove '\n' from last char in array
	char *editedTime = "";
	editedTime = asctime(time);
	editedTime[strlen(editedTime) - 1] = '\0';

	// write timestamp -> temperature -> pressure to file in json style
	fprintf(*dataFile, "{\"timestamp\":\"%s\",", editedTime);
	fprintf(*dataFile, "\"temperature\":%.1f,", *temp);
	fprintf(*dataFile, "\"pressure\":%.1f}\n", *pres);

	return 0;
}