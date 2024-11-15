# Paths to library source files
BMP180 = lib/bmp180driver.c
SENSOR = lib/sensor.c
MQTT = lib/mqtt.c
JSON = lib/cJSON.c

# Compiler and flags
CC = gcc
CFLAGS = -Wall

# Target executable name
TARGET = main.o

# Libraries required
LIBS = -lpaho-mqtt3c -lpaho-mqtt3cs -lssl -lcrypto -lm -li2c

all: $(TARGET)

# Rule to build the main executable
$(TARGET): $(BMP180) $(SENSOR) $(MQTT) $(JSON) main.c
	$(CC) $(CFLAGS) $(BMP180) $(SENSOR) $(MQTT) $(JSON) main.c -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET) > /dev/null 2>&1