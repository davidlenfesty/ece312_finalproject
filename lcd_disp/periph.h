/**
 * @mainpage
 * Peripherals
 * 
 * @brief Peripheral initialization functions and interrupt handling routines.
 * 
 * 
 * Contains the ISRs to handle the USART RX interrupt and the display timer interrupt.
 */


#ifndef PERIPH_H_
#define PERIPH_H_

#include "main.h"


#define USART_BAUDRATE 9600     //! USART baudrate, change this to set it.
#define UBRR_VALUE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

#define BUTTONS_DDR             DDRD
#define BUTTONS_PORT            PORTD
#define BUTTONS_LEFT_PIN        PORTD2
#define BUTTONS_RIGHT_PIN       PORTD3
#define BUTTONS_PLAYPAUSE_PIN   PORTD4


/** @brief Sets up various GPIO functions
 * 
 * First enables buttons as inputs.
 */
void gpio_init();

/** @brief Initialises timer for updating display
 * 
 * Sets up timer0 in
 */
void timer_init();

/** @brief Sets up USART for TX and RX with a baud rate of USART_BAUDRATE
 */
void usart_init();


#endif