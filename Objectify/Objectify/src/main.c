/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <delay.h>
#include <stdio.h>
#include <stdio_serial.h>
#include <spi.h>
#include <spi_master.h>

//ultrasound
#define trig PIO_PC26_IDX
#define echo PIO_PC25_IDX
#define led	 PIO_PC24_IDX
#define STATUS_FOUND 1
#define STATUS_SEARCHING 0
#define STATUS_MAPPING 2
#define RIGHT 0
#define LEFT 1
#define STOP 2
//#define STATUS_MAPPING 2
//ultrasound
//"scaling" �r sammans�ttning av tiden f�r ljudets f�rd i "width-ticks" per mikro_s (3.64) g�nger 2 (p.g.a ljudets str�cka �r dubbbelt
//s� l�ng �n avst�ndet som ska m�tas) och sedan delas detta med ljudets hastighet i cm / us (0.034).
// 'scaling' �r allts� (3.64 * 2) / 0.34 = 214.
#define scaling 214;
uint8_t preDist = 0;
uint8_t mappingDist = 0;
uint8_t mappingDir = 0;
uint8_t mappingDeg = 0;
uint8_t status = STATUS_SEARCHING;
uint8_t deg = 0;
int8_t dir = 0;
uint16_t sum = 0;
uint16_t tick = 0;
uint8_t errors = 0;
void servoControll(unsigned long dist);

void configureConsole()
{
	const usart_serial_options_t uart_serial_options = 
	{
		.baudrate = 115200,
		.paritytype = UART_MR_PAR_NO
	};
	
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONSOLE_UART, &uart_serial_options);
}


long pulseIn()
{
	//printf("pulseread");
	//timeout zone
	unsigned long numloops = 0;
	unsigned long maxloops = 500000;
	unsigned long width = 0;
	// wait for the pulse to start
	while (ioport_get_pin_level(echo)== LOW)
	{
		if (numloops++ == maxloops)
		{
			printf("brokenLOW");
			break;
		}
	}
	
	// wait for the pulse to stop @ here we are measuring the pulse width = incrementing the WIDTH value by one each cycle. atmega328 1 micro second is equal to 16 cycles.
	while (ioport_get_pin_level(echo)==HIGH)
	{
		if (numloops++ == maxloops) 
		{
			printf("brokenHIGH");
			break;
		}
		width++;
	}
	return width/scaling;
}

void testingUltraSound()
{
		unsigned long distance;
	

		
		ioport_set_pin_level(trig, LOW);
		delay_us(2);
		ioport_set_pin_level(trig, HIGH);
		delay_us(10);
		ioport_set_pin_level(trig, LOW);
		distance = pulseIn();
		//distance = 0;
		//printf("%d cm\n", distance);
		servoControll(distance);
		
		
}
void servoControll(unsigned long dist)
{
	deg++;
	switch(status){
		case STATUS_SEARCHING:
		if(preDist>0 && dist <= (preDist/2) && dist<50)
		{
			mappingDist = dist;
			status = STATUS_MAPPING;
			//sum = deg;
			tick++;
			printf("Mapping started");
		}
		break;
		
		case STATUS_MAPPING:
		
		if(dist >= (mappingDist*0.50) && dist <= (mappingDist*1.50)){
			tick++;
			//sum = sum + deg;
			
		}
		printf("%d ", deg);
		
			if(tick>5){
			status = STATUS_FOUND;
			mappingDir = dir;
			mappingDeg = deg;
			dir = STOP;
			printf("FOUND SOMETHING");
			tick = 0;
			} else if(deg == 20)
			{
			tick = 0;
			status = STATUS_SEARCHING;
			printf("Started Search");
			}
		break;
		
		case STATUS_FOUND:
			
		if(dist < (mappingDist*0.50) || dist > (mappingDist*1.50))
		{
			errors++;
			if(errors >5)
			{
				errors = 0;
				deg = mappingDeg;
				dir = mappingDir;
				
				printf("SEARCHING");
				status = STATUS_SEARCHING;
			}
			break;
		}
		dir = STOP;
		printf("FOUND");
		errors = 0;
		break;
	}
	

	if(deg == 20)
	{
		if(dir == LEFT) dir = RIGHT;
		else if(dir== RIGHT) dir = LEFT;
		deg = 0;
	}
	preDist = dist;
	}


void testingRFID()
{
	//NPCS0
	
	
}

int main (void)
{
	ioport_set_pin_dir(trig,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(echo, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(led, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(led,LOW);
	//turn of watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	//init clock
	sysclk_init();
	//init board
	board_init();
	//init serial communication, printf ..
	configureConsole();
	//test ultrasound sensor
	tick = 20;
	while(1)
	{	
		switch(dir){
		case STOP:
			ioport_set_pin_level(led, HIGH);
			delay_us(1500);
			ioport_set_pin_level(led, LOW);
			delay_us(100000);
			break;
	
		case RIGHT:
		//printf("SEARCHING DIR 0");
		ioport_set_pin_level(led, HIGH);
		delay_us(1671);
		ioport_set_pin_level(led, LOW);
		delay_us(100000);
		break;
		
		case LEFT:
		//printf("SEARCHING DIR 1");
		ioport_set_pin_level(led, HIGH);
		delay_us(1300); 
		ioport_set_pin_level(led, LOW);
		delay_us(100000);
		break;
	}
	//testingUltraSound();
	
	/**TESTNING AV SYMMETRISK VRIDNING.
	**/
	if(tick == 0){
		tick = 20;
		if(dir==LEFT) dir = RIGHT;
		else dir = LEFT;
		//dir = STOP;
	}
	tick--;
	//**/
	}
	
	//test rfid
	//testingRFID();
	
	

	/* Insert application code here, after the board has been initialized. */

}
