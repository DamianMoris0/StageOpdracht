BMP180=lib/bmp180driver
SENSOR=lib/sensor

all:
	gcc -Wall -c $(BMP180).c -o $(BMP180).o -lm -li2c
	gcc -Wall -c $(SENSOR).c -o $(SENSOR).o -lm -li2c
	gcc -Wall $(BMP180).o $(SENSOR).o main.c -o main.o -lm -li2c

clean:
	rm *.o > /dev/null 2>&1 &
