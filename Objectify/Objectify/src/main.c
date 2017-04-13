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

//ultrasound
//"scaling" �r sammans�ttning av tiden f�r ljudets f�rd i "width-ticks" per mikro_s (3.64) g�nger 2 (p.g.a ljudets str�cka �r dubbbelt
//s� l�ng �n avst�ndet som ska m�tas) och sedan delas detta med ljudets hastighet i cm / us (0.034).
// 'scaling' �r allts� (3.64 * 2) / 0.34 = 214.
#define scaling 214;


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
	
	ioport_set_pin_dir(trig,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(echo, IOPORT_DIR_INPUT);
	unsigned long distance;
	while(1)
	{
		
		ioport_set_pin_level(trig, LOW);
		delay_us(2);
		ioport_set_pin_level(trig, HIGH);
		delay_us(10);
		ioport_set_pin_level(trig, LOW);
		distance = pulseIn();
		printf("%d cm\n", distance);
		delay_ms(500);
	}	
}

#define SPI_Handler     SPI0_Handler
#define SPI_IRQn SPI0_IRQn

/** Spi Hw ID . */
#define SPI_ID ID_SPI0

//spio address
#define SPI_MASTER_BASE SPI0

/** SPI base address for SPI slave mode, (on different board) */
#define SPI_SLAVE_BASE SPI0

#define SPI_CHIP_SEL 0 //use spi chip select 0
#define SPI_CHIP_PCS spi_get_pcs(SPI_CHIP_SEL)
//clock polarity
#define SPI_CLK_POLARITY 0
//clock phase
#define SPI_CLK_PHASE 0
/* Delay before SPCK. */
//#define SPI_DLYBS 0x40
#define SPI_DLYBS 0xFF

/* Delay between consecutive transfers. */
#define SPI_DLYBCT 0x10
/* SPI clock setting (Hz). */
static uint32_t gs_ul_spi_clock = 1000000;


void testingRFID()
{
	//NPCS0
	
	struct spi_device spi_dev ={
		.id=IOPORT_CREATE_PIN(PIO_PA28_IDX,1)
		};
	spi_master_init(SPI_MASTER_BASE);
	spi_master_setup_device(SPI_MASTER_BASE,)
	
}

int main (void)
{
	//turn of watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;

	//init clock
	sysclk_init();
	//init board
	board_init();
	//init serial communication, printf ..
	configureConsole();
	
	//test ultrasound sensor
	//testingUltraSound();
	
	//test rfid
	testingRFID();
	
	

	/* Insert application code here, after the board has been initialized. */

}
