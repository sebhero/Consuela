
#include <asf.h>

int main (void) {
	sysclk_init();

	board_init();

    // Start the scheduler.
    // Again this function should never return
    vTaskStartScheduler();

	for(;;) {

    }
}
