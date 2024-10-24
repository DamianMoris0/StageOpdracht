BMP180=lib/bmp180driver
SENSOR=lib/sensor
MQTT=lib/mqtt

all:
	gcc -Wall -c $(BMP180).c -o $(BMP180).o -lm -li2c
	gcc -Wall -c $(SENSOR).c -o $(SENSOR).o -lm -li2c
	gcc -Wall -c $(MQTT).c -o $(MQTT).o -lpaho-mqtt3cs -lssl -lcrypto

	gcc -Wall $(BMP180).o $(SENSOR).o $(MQTT).o main.c -o main.o -lm -li2c -lpaho-mqtt3cs -lssl -lcrypto

clean:
	rm *.o > /dev/null 2>&1 &
