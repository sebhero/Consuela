

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_
#include <asf.h>

void configure_console(void);
void configure_usart(Pdc **pdc_to_Set, xQueueHandle sem_to_use);




#endif /* COMMUNICATION_H_ */