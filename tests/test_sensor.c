#include "../unity/unity.h"
#include "../lib/bmp180driver.h"
#include "../lib/sensor.h"

void setUp(void) {}
void tearDown(void) {}

void test_initSensor(void)
{
	/* Init test sensors */
	void *testSensor1 = bmp180_init(0x77, "/dev/i2c-1");
	void *testSensor2 = bmp180_init(0x36, "/dev/i2c-1");
	void *testSensor3 = bmp180_init(0x77, "/fesgsdfeg");
	void *testSensor4 = bmp180_init(0x19, "/mppzeplfz");
	void *testSensor5 = bmp180_init(0x77, "/dev/i2c-1");

	/* Test the declared sensors */
	TEST_ASSERT_EQUAL_MESSAGE(0, initSensor(&testSensor1, BMP180), "Normal BMP180");
	TEST_ASSERT_EQUAL_MESSAGE(0, initSensor(&testSensor2, BMP180), "Other sensor address");
	TEST_ASSERT_EQUAL_MESSAGE(0, initSensor(&testSensor3, BMP180), "Wrong I2C path");

	TEST_ASSERT_EQUAL_MESSAGE(0, initSensor(&testSensor4, BMP180), "Wrong sensor address and I2C path");
	TEST_ASSERT_EQUAL_MESSAGE(0, initSensor(&testSensor5, 1),	   "Non existend sensor type");
	
}

void test_createJsonFromSensorData(void)
{
	/* Declare some test sensor data structs */
	struct SensorValues sensorData1 = {.id = "Sensor_1",.typeStr = "BMP180",.typeID = BMP180,.time = "13:01:24",.temperature = 21.3,.pressure = 1009.6};
	struct SensorValues sensorData2 = {.id = "Sensor_2",.typeStr = "DEFAULT_SENSOR",.typeID = DEFAULT_SENSOR,.time = "13:01:24",.temperature = 21.3,.pressure = 1009.6};
	struct SensorValues sensorData3 = {.id = "Sensor_3",.typeStr = "BMP180",.pressure = 1009.6};
	struct SensorValues sensorData4 = {.id = "Sensor_4"};
	struct SensorValues sensorData5 = {};


	/* Check it sensor values are correctly converted into the right json string */
	TEST_ASSERT_EQUAL_STRING_MESSAGE("{sensorID=\"Sensor_1\",sensorType=\"BMP180\",timestamp=\"13:01:24\",temperature=21.3,pressure=1009.6}",
									 createJsonFromSensorData(&sensorData1), "Normal sensor data to json string conversion");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("{sensorID=\"Sensor_2\",sensorType=\"DEFAULT_SENSOR\",timestamp=\"13:01:24\",temperature=21.3,pressure=1009.6}",
									 createJsonFromSensorData(&sensorData2), "Normal sensor data to json string conversion");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("{sensorID=\"Sensor_3\",sensorType=\"BMP180\",timestamp=\"?\",temperature=,pressure=1009.6}",
									 createJsonFromSensorData(&sensorData3), "No timestamp and temperature in sensor values");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("{sensorID=\"Sensor_4\",sensorType=\"?\",timestamp=\"?\",temperature=21.3,pressure=1009.6}",
									 createJsonFromSensorData(&sensorData4), "No sensor type string, timestamp, temperature and pressure");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("{sensorID=\"?\",sensorType=\"?\",timestamp=\"?\",temperature=,pressure=}",
									 createJsonFromSensorData(&sensorData5), "Empty struct");									 
}

void test_writeSensorDataToFile(void)
{
	/* more test stuff */
}

void test_checkDataBounds(void)
{
	/* Check normal circumstances */
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0, checkDataBounds(21.3, 1013.8), "Temperature and pressure within bounds");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0, checkDataBounds(-38.1, 889),   "Temperature and pressure within bounds");

	/* Check long floating point size */
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0, checkDataBounds(21.123456789123456789123456789, 1013), "Temperature big floating point size");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0, checkDataBounds(21, 1013.123456789123456789123456789), "Pressure big floating point size");

	/* Check out of lower bounds */
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0, checkDataBounds(-200, 1013), "Temperature out of lower bound");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1, checkDataBounds(21, 500.5),  "Pressure out of lower bound");

	/* Check out of upper bound */
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1, checkDataBounds(321.0, 1013), "Temperature out of upper bound");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1, checkDataBounds(21, 1500), 	 "Pressure out of upper bound");
}

void test_getSensorTypeName(void)
{
	/* Test if returned sensor name strings correspond with the given type */
	TEST_ASSERT_EQUAL_STRING_MESSAGE("BMP180", 		   getSensorTypeName(BMP180), "Sensor type returns correct name");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("BMP180", 		   getSensorTypeName(0), 	  "Sensor type returns correct name");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("DEFAULT_SENSOR", getSensorTypeName(1), 	  "Sensor type returns default");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("DEFAULT_SENSOR", getSensorTypeName(222), 	  "Sensor type returns default");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("DEFAULT_SENSOR", getSensorTypeName(-1), 	  "Sensor type returns default");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("DEFAULT_SENSOR", getSensorTypeName(0.0246), "Sensor type returns default");
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_initSensor);
	RUN_TEST(test_createJsonFromSensorData);
	//RUN_TEST(test_writeSensorDataToFile);
	RUN_TEST(test_checkDataBounds);
	RUN_TEST(test_getSensorTypeName);
	return UNITY_END();
}
