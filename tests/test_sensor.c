#include "../unity/unity.h"
#include "../lib/sensor.h"
#include "../lib/bmp180driver.h"

void setUp(void) {}
void tearDown(void) {}

void test_initSensor(void)
{
	/* Init test sensors */
	/*void *testSensor1 = bmp180_init(0x77, "/dev/i2c-1");
	void *testSensor2 = bmp180_init(0x36, "/dev/i2c-1");
	void *testSensor3 = bmp180_init(0x77, "/fesgsdfeg");
	void *testSensor4 = bmp180_init(0x19, "/mppzeplfz");

	TEST_ASSERT_EQUAL_MESSAGE(0, initSensor(&testSensor1), "Normal BMP180");
	TEST_ASSERT_EQUAL_MESSAGE(0, initSensor(&testSensor2), "Wrong sensor address");
	TEST_ASSERT_EQUAL_MESSAGE(0, initSensor(&testSensor3), "Wrong I2C path");
	TEST_ASSERT_EQUAL_MESSAGE(0, initSensor(&testSensor4), "Wrong sensor address and I2C path");*/
}

void test_writeSensorDataToFile(void)
{
	/* more test stuff */
}

void test_checkDataBounds(void)
{
	/* Check normal circumstances */
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0, checkDataBounds(21.3, 1013.8), "Temperature and pressure within bounds");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0, checkDataBounds(-38.1, 889), "Temperature and pressure within bounds");

	/* Check long floating point size */
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0, checkDataBounds(21.123456789123456789123456789, 1013), "Temperature big floating point size");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0, checkDataBounds(21, 1013.123456789123456789123456789), "Pressure big floating point size");

	/* Check out of lower bounds */
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1, checkDataBounds(-200, 1013), "Temperature out of lower bound");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1, checkDataBounds(21, 500.5), "Pressure out of lower bound");

	/* Check out of upper bound */
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1, checkDataBounds(321.0, 1013), "Temperature out of upper bound");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1, checkDataBounds(21, 1500), "Pressure out of upper bound");
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_initSensor);
	RUN_TEST(test_writeSensorDataToFile);
	RUN_TEST(test_checkDataBounds);
	return UNITY_END();
}
