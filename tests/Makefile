UNITY_ROOT = ../unity

# Compiler flags
#CFLAGS=-std=c89
CFLAGS += -Wall

# Libraries to link
LIBS = -lpaho-mqtt3cs -lssl -lcrypto -lm -li2c -lcurl

# Source files and include directories
SRC_FILES_MQTT = $(UNITY_ROOT)/unity.c $(UNITY_ROOT)/unity_testrail.c ../lib/bmp180driver.c ../lib/sensor.c ../lib/cJSON.c ../lib/mqtt.c test_mqtt.c
SRC_FILES_SENSOR = $(UNITY_ROOT)/unity.c $(UNITY_ROOT)/unity_testrail.c ../lib/bmp180driver.c ../lib/sensor.c ../lib/cJSON.c ../lib/mqtt.c test_sensor.c
INC_DIRS = -Isrc -I$(UNITY_ROOT)

# Targets
TARGET_MQTT = test_mqtt.o
TARGET_SENSOR = test_sensor.o

all: $(TARGET_MQTT) $(TARGET_SENSOR)

# Rule to build test_mqtt.o
$(TARGET_MQTT): $(SRC_FILES_MQTT)
	gcc $(CFLAGS) $(INC_DIRS) $(SRC_FILES_MQTT) -o $(TARGET_MQTT) $(LIBS)

# Rule to build test_sensor.o
$(TARGET_SENSOR): $(SRC_FILES_SENSOR)
	gcc $(CFLAGS) $(INC_DIRS) $(SRC_FILES_SENSOR) -o $(TARGET_SENSOR) $(LIBS)

clean:
	rm -f $(TARGET_MQTT) $(TARGET_SENSOR)
