#include <stdio.h>
#include <time.h>

int initSensor(void *sensor);
int writeSensorDataToFile(FILE **dataFile, float *temp, float *pres, struct tm *time);