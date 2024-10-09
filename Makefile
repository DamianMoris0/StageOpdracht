SENSOR=bmp180/src/bmp180

all:
	gcc -Wall -c $(SENSOR).c -o $(SENSOR).o -lm -li2c
	gcc -Wall $(SENSOR).o sensor.c -o sensor.o -lm -li2c

clean:
	rm *.o > /dev/null 2>&1 &
