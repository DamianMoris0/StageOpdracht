#include <stdio.h>
#include <time.h>

#define BMP180 0

int initSensor(void *sens, int TYPE);
int writeSensorDataToFile(FILE **dataFile, float *temp, float *pres, struct tm *time);
int checkDataBounds(float temp, float pres);