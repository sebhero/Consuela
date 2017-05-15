/*
 * Controller.c
 *
 * Created: 2017-05-08 23:58:42
 *  Author: Danial Mahmoud
 */ 

#include <asf.h>
#include <stdio.h>
#include <FreeRTOSConfig.h>


enum state{
	START,
	DRIVING,
};

typedef enum state state_t;

void vControllerTask(void *pvParameters){
	state_t current_state = START;
	state_t next_state = START;
	while (1)
	{
		switch (current_state)
		{
			case START:
			     
			break;
			
			case DRIVING:
			break;
		}
	}
}