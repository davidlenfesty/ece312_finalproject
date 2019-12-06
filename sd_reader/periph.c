#include "main.h"
#include <avr/interrupt.h>

/** @brief intercepts incoming Serial commands
 */
ISR(USART_RX_vect) {
    cli();
    // Read byte into FIFO
    fifo_push(UDR0);

    // Flag is cleared by reading data from UDR
    sei();
}

/** @brief Interrupt handler for actually playing music
 * 
 * Will only be called if the TIFR OCF0A flag is high, using that as our play/pause flag
 * 
 * This may be possible to hand-optimise better, but I cut it down to ~40 instructions
 * which should be fine at 16MHz
 */
ISR(TIMER0_COMPA_vect) {
    cli();
    // Handle music playing
    /* DAC0 -> DAC5 are on PC0 -> PC5
     * Nothing relevent is on the rest of PORTC
     * 
     * DAC6 -> DAC11 is PD2->PD7 
     * 
     * DAC12 and DAC13 are PB0 and PB1
     */
    uint16_t in_data = song_buf[song_buf_select][song_position];
    // Split into 8 bit because we are using an 8 bit MCU, so code can be better
    uint8_t data_lo = in_data;
    uint8_t data_hi = in_data >> 8;

    // DAC0 -> DAC5
    PORTC = (data_lo >> 2) & 0x3F;
    // DAC6 -> DAC11
    PORTD &= 0x03;
    PORTD |= data_hi << 2;
    // DAC12 and DAC13
    PORTB &= 0xFC; // clear bits
    PORTB |= data_hi >> 6;

    // Go to next sample
    song_position++;
    // Can just wrap if it's 8 bit
    sei();
}

void gpio_init() {
    // Initialise all DAC outputs
    /* DAC0 -> DAC5 are on PC0 -> PC5
     * Nothing relevent is on the rest of PORTC
     * 
     * DAC6 -> DAC11 is PD2->PD7 
     * 
     * DAC12 and DAC13 are PB0 and PB1
     */
    DDRC |= 0x3F;
    DDRD |= 0xFC;
    DDRC |= 0x03;
}

void timer_init() {
    // Compare outputs should be default, nothing
    // Set to CTC, WGM = 0b010
    TCCR0A = (1 << OCIE0A);

    // Set timer to use clock input w/o prescaling
    TCCR0B = 1;

    // 16 MHz / 44kHz = 182 clock cycles
    // Pushing it for time, may have to move to 8 bit...
    OCR0A = 182;

    // Don't enable the interrupt yet
}

void usart_init() {
    // set baudrate
    UBRR0L = UBRR_VALUE & 255;
    UBRR0H = UBRR_VALUE >> 8;

    // Set data info, no parity, 1 stop, 8-bit
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

    // Start it up
    UCSR0B |= (1 << TXEN0) | (1 << RXEN0);
}