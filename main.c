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
	void *sensor = bmp180_init(addr, i2c);
	time_t rawtime;
	struct tm *timeinfo;
	FILE *sensorDataFile;

	// initialise sensor
	initSensor(&sensor, BMP180);

	// open data.json file
	sensorDataFile = fopen("/var/data/data.json", "w");
	if (sensorDataFile == NULL) {
		printf("Unable to open file\n");
		return 1;
	}

	// read sensor data and write to file
	for (int i = 0; i < 100; i++) {							// this will become an infinite loop when database structure is finished
		float temperature = bmp180_temperature(sensor);
		float pressure = bmp180_pressure(sensor) / 100; 	// divide by 100 to get hPa
		checkDataBounds(temperature, pressure);				// if temperature or pressure are out of bounds return 1
		time(&rawtime);										// get raw time date to later convert into date-time format
		timeinfo = localtime(&rawtime);
		//printf("%sTemperature = %.1f °C, Pressure = %.1f hPa\n\n", asctime(timeinfo), temperature, pressure);
		writeSensorDataToFile(&sensorDataFile, &temperature, &pressure, timeinfo);
		usleep(100000); // time in µs
	}

	// close file and bmp180
	fclose(sensorDataFile);
	bmp180_close(sensor);

	return 0;
}
