/*
 * navigationTest.c
 *
 * Created: 10/05/2017 08:19:57
 *  Author: Robin_2
 */ 
#include <asf.h>
#include <inttypes.h>
#include "navigationTest.h"
#include "../unity/unity.h"
#include "navigation.h"

double foo;
int angleValue;

void test_setX1(void){
	x1_pos = 255;
	TEST_ASSERT_EQUAL_UINT16(255,x1_pos);
	TEST_ASSERT_NOT_EQUAL(256,x1_pos);
	TEST_ASSERT_NOT_EQUAL(254,x1_pos);
	x1_pos = 0;
}

void test_setY1(void){
	y1_pos = 255;
	TEST_ASSERT_EQUAL_UINT16(255,y1_pos);
	TEST_ASSERT_NOT_EQUAL(256,y1_pos);
	TEST_ASSERT_NOT_EQUAL(254,y1_pos);
	y1_pos = 0;
}

void test_distanceToPosition0(void){
	foo = distanceToPosition(0);
	TEST_ASSERT_EQUAL_UINT16(200,foo);
	TEST_ASSERT_NOT_EQUAL(256,foo);
}

void test_distanceToPosition1(void){
	foo = distanceToPosition(1);
	TEST_ASSERT_EQUAL_UINT16(400,foo);
	TEST_ASSERT_NOT_EQUAL(256,foo);
}

void test_distanceToPosition2(void){
	foo = distanceToPosition(2);
	TEST_ASSERT_EQUAL_UINT16(500,foo);
	TEST_ASSERT_NOT_EQUAL(256,foo);
}

void test_distanceToPosition3(void){
	foo = distanceToPosition(3);
	TEST_ASSERT_EQUAL_UINT16(500,foo);
	TEST_ASSERT_NOT_EQUAL(256,foo);
}

void test_AngleToPos0(){
	initNav();
	angleValue = angleToPos();
	TEST_ASSERT_EQUAL_INT(-90,angleValue);
	TEST_ASSERT_NOT_EQUAL(-91,angleValue);
	TEST_ASSERT_NOT_EQUAL(-89,angleValue);
}

void test_AngleToPos1(){
	initNav();
	angleValue = angleToPos();
	TEST_ASSERT_EQUAL_INT(360,angleValue);
	TEST_ASSERT_NOT_EQUAL(1,angleValue);
	TEST_ASSERT_NOT_EQUAL(359,angleValue);
}
void test_AngleToPos2(){
	initNav();
	angleValue = angleToPos();
	TEST_ASSERT_EQUAL_INT(-37,angleValue);
	TEST_ASSERT_NOT_EQUAL(-38,angleValue);
	TEST_ASSERT_NOT_EQUAL(-36,angleValue);
}
void test_AngleToPos3(){
	initNav();
	angleValue = angleToPos();
	TEST_ASSERT_EQUAL_INT(-54,angleValue);
	TEST_ASSERT_NOT_EQUAL(-55,angleValue);
	TEST_ASSERT_NOT_EQUAL(-53,angleValue);
}
