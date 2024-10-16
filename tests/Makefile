UNITY_ROOT=../unity

#CFLAGS=-std=c89
CFLAGS+=-Wall

TARGET_BASE=test_sensor
TARGET=$(TARGET_BASE).o
SRC_FILES=$(UNITY_ROOT)/unity.c ../lib/bmp180driver.c ../lib/sensor.c test_sensor.c
INC_DIRS=-Isrc -I$(UNITY_ROOT)

all: $(SRC_FILES)
	gcc $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -o $(TARGET) -lm -li2c
	- ./$(TARGET)

clean:
	rm -f $(TARGET)
