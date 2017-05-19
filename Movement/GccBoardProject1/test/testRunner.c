/*
 * testRunner.c
 *
 * Created: 10/05/2017 09:06:22
 *  Author: Robin_2
 */ 

#include <asf.h>
#include <inttypes.h>
#include <stdio_serial.h>
#include "conf_board.h"
#include "../src/consoleFunctions.h"
#include "../unity/unity.h"

#include "navigationTest.h"

void setUp(void);
void tearDown(void);
void resetTest(void);
/*
int main(void){
	sysclk_init();
	board_init();
	configureConsole();
	/*
	//Tests navigation functions
	UnityBegin("../test/navigationTest.c");
	RUN_TEST(test_setX1,4);
	RUN_TEST(test_setY1,8);
	RUN_TEST(test_distanceToPosition0,16);
	RUN_TEST(test_AngleToPos0, 18);
	RUN_TEST(test_distanceToPosition1,20);
	RUN_TEST(test_AngleToPos1, 22);
	RUN_TEST(test_distanceToPosition2,24);
	RUN_TEST(test_AngleToPos2, 26);
	RUN_TEST(test_distanceToPosition3,28);
	RUN_TEST(test_AngleToPos3, 30);
	UnityEnd();
	
	return 0;
}*/

void setUp(void)
{
	/* Do nothing */
}

void tearDown(void)
{
	/* Do nothing */
}

void resetTest(void)
{
	tearDown();
	setUp();
}