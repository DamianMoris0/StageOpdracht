all:
	gcc -o mqtt_test.o mqtt_test.c -lpaho-mqtt3cs -lssl -lcrypto
	- ./mqtt_test.o

clean:
	rm -f mqtt_test.o
