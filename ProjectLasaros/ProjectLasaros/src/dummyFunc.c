/*
 * dummyFunc.c
 *
 * Created: 2017-05-18 09:25:54
 *  Author: Spellabbet
 */ 
#include <inttypes.h>
#include "dummyFunc.h"
#include "navigation.h"
#include "TwiComHandler.h"

int _getall = 0;
uint8_t _isDonePickup=0;
uint8_t _isDoneDropoff=0;
int currentObj=0;
int nextPos=0;

typedef struct {
	uint16_t x_pos;
	uint16_t y_pos;
	const char *name_p;
} object_pos_t;


object_pos_t _objects[] = {
	{
		.x_pos = sock_x,
		.y_pos = sock_y,
		.name_p = "Sock"
	},
	{
		.x_pos = glass_x,
		.y_pos = glass_y,
		.name_p = "Glass"
	},
	{
		.x_pos = cube_x,
		.y_pos = cube_y,
		.name_p = "Cube"
	},
	{
		.x_pos = dropOff_x,
		.y_pos = dropOff_y,
		.name_p = "Drop off"
	}
};

uint8_t isDropOff()
{
	if(nextPos>=3)
	{
		return 1;
	}
	return 0;
}

uint8_t goToNext(void){
	//decied were to go
	if(_isDonePickup)
	{
		puts("_isDonePickup");

		if(_getall)
		{
			//go to next
			currentObj++;
			nextPos=currentObj;
			//if at dropoff
			if(nextPos >=3)
			{
				currentObj=3;
				return 2;
			}
			
		}
		else{
			//go to drop off
			nextPos=3;
		}
		_isDonePickup=0;
		
		//do driving loop
		printf("Driving to %d\n",nextPos);
		//returns 0;
		//delay...vTaskDelay
		printf("Arrived AT %d\n",nextPos);
		//when done
		return 1;
		
	}
	if(_isDoneDropoff)
	{
		puts("_isDoneDropoff");
		if(_getall == 0)
		{
			//goto next
			currentObj++;
			nextPos=currentObj;
		}
		else{
			//FINISHED!
		}
		_isDoneDropoff=0;
		
		//do driving loop
		printf("Driving to %d\n",nextPos);
		//returns 0;
		//delay...vTaskDelay
		printf("Arrived AT %d\n",nextPos);
		//when done
		return 2;
	}
	return 1;
}

void setObject(Object object,int x, int y)
{
	switch(object)
	{
		case GLASS:
		_objects[1].x_pos=x;
		_objects[1].y_pos=y;
		break;
		case SOCK:
		_objects[0].x_pos=x;
		_objects[0].y_pos=y;
		break;
		case SQUARE:
		_objects[2].x_pos=x;
		_objects[2].y_pos=y;
		break;
		case BOXGOAL:
		_objects[3].x_pos=x;
		_objects[3].y_pos=y;
		break;
	}
}

setGetAll(uint8_t getall)
{
	printf("collectAll= %d",getall);
	_getall=getall;
}

void setDonePickup(void)
{
	puts("setDonePickup\n");
	_isDonePickup=1;
}

void setDoneDropoff(void)
{
	puts("setDoneDropoff\n");
	_isDoneDropoff=1;
}