#ifndef PERIPH_H_
#define PERIPH_H_

#define USART_BAUDRATE 9600     //! USART baudrate, change this to set it.
#define UBRR_VALUE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

// TODO comments

void gpio_init();
void timer_init();
void usart_init();

#endif