#include "lib/bmp180driver.h"
#include "lib/sensor.h"
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int main()
{
	// initialise variables
	char *i2c = "/dev/i2c-1";
	int addr = 0x77;
	void *bmp180 = bmp180_init(addr, i2c);
	time_t rawtime;
	struct tm *timeinfo;
	FILE *sensorDataFile;

	// initialise bmp180
	initSensor(&bmp180);

	// open data.json file
	sensorDataFile = fopen("/var/data/data.json", "w");
	if (sensorDataFile == NULL) {
		printf("Unable to open file\n");
		return 1;
	}

	// read sensor data and write to file
	for (int i = 0; i < 100; i++) {
		float temperature = bmp180_temperature(bmp180);
		if (temperature <= -50.0 || temperature >= 50) {
			printf("Temperature out of bounds\n");
			return 1;
		}
		float pressure = bmp180_pressure(bmp180) / 100;
		if (pressure <= 800 || pressure >= 1200) {
			printf("Pressure out of bounds\n");
			return 1;
		}
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		//printf("%sTemperature = %.1f °C, Pressure = %.1f hPa\n\n", asctime(timeinfo), temperature, pressure);
		writeSensorDataToFile(&sensorDataFile, &temperature, &pressure, timeinfo);
		usleep(100000); // in µs
	}

	// close file and bmp180
	fclose(sensorDataFile);
	bmp180_close(bmp180);

	return 0;
}
