#include <stdio.h>
#include <time.h>

#define BMP180 0

struct SensorValues {
    int type;
    char* id;
    char* time;
    float temperature;
    float pressure;
};

int initSensor(void *sens, int type);
int writeSensorDataToFile(FILE **dataFile, struct SensorValues *sensorValues);
int checkDataBounds(float temp, float pres);