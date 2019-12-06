#include "periph.h"


/** @brief Handles incoming Serial commands from the main player.
 */
ISR(USART_RX_vect) {
    cli();
    // Push incoming data to FIFO
    fifo_push(UDR);

    // Flag is cleared by reading data from UDR
    sei();
}

/** @brief Display update timer
 */
ISR(TIMER0_COMPA_vect) {
    cli();
    update_display = true;

    // Flag is cleared by calling the vector
    sei();
}

void gpio_init() {
    // Initialise button pins as inputs
    BUTTONS_DDR &=  ~(1 << BUTTONS_LEFT_PIN) &
                    ~(1 << BUTTONS_RIGHT_PIN) &
                    ~(1 << BUTTONS_PLAYPAUSE_PIN);
    // Turn on button pullups
    BUTTONS_PORT |= (1 << BUTTONS_LEFT_PIN) |
                    (1 << BUTTONS_RIGHT_PIN) |
                    (1 << BUTTONS_PLAYPAUSE_PIN);
}

void timer_init() {
    /**
     * Need to verify the math here.
     * But with a prescaler of 1024, f_cpu at 8MHz, an OCRA of 195 gives
     * 20Hz refresh rate.
     */

    // COM0x can be set to defaults
    // To set to CTC, WGM = 0b010
    TCCR0A = (1 << WGM01);

    // CS = 0b101 -> 1024 prescaler
    // Everything else should be 0.
    // TCCR0B = 0b00000101; <- can't use this because binary stuff is a GCC extension
    TCCR0B = 5;

    // Set to ~20Hz
    OCR0A = 195;

    // Enable interrupt
    TIMSK = (1 << OCIE0A);
}

void usart_init() {
    // initialize USART
	UBRRL = UBRR_VALUE & 255; 
	UBRRH = UBRR_VALUE >> 8;
	UCSRB = (1 << TXEN) | (1 << RXEN); // fire-up USART
	UCSRC = (1 << UCSZ1) | (1 << UCSZ0); // fire-up USART


    // Enable RX complete interrupt
    UCSRB |= (1 << RXCIE);
}