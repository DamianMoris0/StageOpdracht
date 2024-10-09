#include "bmp180/src/bmp180.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int writeSensorDataToFile(FILE **dataFile, float *temp, float *pres, struct tm *time);

int main ()
{
	char *i2c = "/dev/i2c-1";
	int addr = 0x77;
	void *bmp180 = bmp180_init(addr, i2c);
	time_t rawtime;
	struct tm *timeinfo;
	FILE *sensorDataFile;

	bmp180_eprom_t eprom;
	bmp180_dump_eprom(bmp180, &eprom);
	bmp180_set_oss(bmp180, 1);
	if (bmp180 == NULL) {
		printf("Unable to detect BMP180\n");
		return 1;
	}

	sensorDataFile = fopen("/var/data/data.json", "w");
	if (sensorDataFile == NULL) {
		printf("Unable to open file\n");
		return 1;
	}

	for (int i = 0; i < 5; i++) {
		float temperature = bmp180_temperature(bmp180);
		float pressure = bmp180_pressure(bmp180) / 100;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		printf("%sTemperature = %.1f °C, Pressure = %.1f hPa\n\n", asctime(timeinfo), temperature, pressure);
		//fprintf(sensorDataFile, "%s", asctime(timeinfo));
		writeSensorDataToFile(&sensorDataFile, &temperature, &pressure, timeinfo);
		usleep(1000000);
	}

	fclose(sensorDataFile);
	bmp180_close(bmp180);
	return 0;
}

int writeSensorDataToFile (FILE **dataFile, float *temp, float *pres, struct tm *time)
{
	fprintf(*dataFile, "{\"timestamp\":\"");
	char *editedTime = "";
	editedTime = asctime(time);
	editedTime[strlen(editedTime) - 1] = '\0';
	fprintf(*dataFile, "%s", editedTime);
	fprintf(*dataFile, "\",\"temperature\":");
	return 0;
}
