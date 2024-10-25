#include <stdio.h>
#include <time.h>

#define BMP180                          0
#define SEND_JSON_DATA_FILE_INTERVAL    1000000 // Time in µs, defines the time intervals after which to send the json data (60.000.000 µs == 1 min)
#define SAMPLE_TIME                     100000  // Time in µs, dont exceed SEND_JSON_DATA_FILE_INTERVAL
#define write_to_json_file_flag         0       // Set to 1 to write to json file, else set 0
#define publish_json_on_mqtt_topic_flag 1       // Set to 1 to publish json on mqtt topic, else set 0

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