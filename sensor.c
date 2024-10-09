#include "bmp180/src/bmp180.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int writeSensorDataToFile(FILE **dataFile, float *temp, float *pres, struct tm *time);

int main ()
{
	// initialise variables
	char *i2c = "/dev/i2c-1";
	int addr = 0x77;
	void *bmp180 = bmp180_init(addr, i2c);
	time_t rawtime;
	struct tm *timeinfo;
	FILE *sensorDataFile;

	// initialise bmp180
	bmp180_eprom_t eprom;
	bmp180_dump_eprom(bmp180, &eprom);
	bmp180_set_oss(bmp180, 1);
	if (bmp180 == NULL) {
		printf("Unable to detect BMP180\n");
		return 1;
	}

	// open data.json file
	sensorDataFile = fopen("/var/data/data.json", "w");
	if (sensorDataFile == NULL) {
		printf("Unable to open file\n");
		return 1;
	}

	// read sensor data and write to file
	for (int i = 0; i < 200; i++) {
		float temperature = bmp180_temperature(bmp180);
		float pressure = bmp180_pressure(bmp180) / 100;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		printf("%sTemperature = %.1f °C, Pressure = %.1f hPa\n\n", asctime(timeinfo), temperature, pressure);
		writeSensorDataToFile(&sensorDataFile, &temperature, &pressure, timeinfo);
		usleep(1000000);
	}

	// close file and bmp180
	fclose(sensorDataFile);
	bmp180_close(bmp180);

	return 0;
}

int writeSensorDataToFile (FILE **dataFile, float *temp, float *pres, struct tm *time)
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
