#include <stdio.h>

#define BMP180                          0
#define DEFAULT_SENSOR                  10

#define SEND_JSON_DATA_FILE_INTERVAL    1000000   // Time in µs, defines the time intervals after which to send the json data (60.000.000 µs == 1 min)
#define SAMPLE_TIME                     10000000  // Time in µs, dont exceed SEND_JSON_DATA_FILE_INTERVAL (for the 8000 messages per day limit, min value = 10400000)
#define write_to_json_file_flag         0         // Set to 1 to write to json file, else set 0
#define publish_json_on_mqtt_topic_flag 1         // Set to 1 to publish json on mqtt topic, else set 0

struct SensorValues {
    char* id;
    char* typeStr;
    int typeID;
    char* time;
    double temperature;
    double pressure;
};

int initSensor(void *sens, int type);
char* createJsonFromSensorData(struct SensorValues *sensVals);
int writeSensorDataToFile(FILE **dataFile, char* jsonSensVals);
int checkDataBounds(double temp, double pres);
char* getSensorTypeName(int type);