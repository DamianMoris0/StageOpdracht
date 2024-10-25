#include <stdio.h>
#include <time.h>

#define BMP180              0
#define SEND_DATA_INTERVAL  1000000    // time in µs, defines the time intervals after which to send the json data (60.000.000 µs == 1 min)
#define SAMPLE_TIME         100000      // time in µs, dont exceed SEND_DATA_INTERVAL

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