#include <stdio.h>
#include <time.h>

#define BMP180              0
#define SEND_DATA_INTERVAL  1000000    // time in µs, defines the time intervals after which to send the json data (60.000.000 µs == 1 min)
#define SAMPLE_TIME         100000     // time in µs, dont exceed SEND_DATA_INTERVAL

struct SensorValues {
    int type;
    char* id;
    char* time;
    double temperature;
    double pressure;
};

int initSensor(void *sens, int type);
char* createJsonFromSensorData(struct SensorValues *sensVals);
int writeSensorDataToFile(FILE **dataFile, char* jsonSensVals);
int checkDataBounds(double temp, double pres);