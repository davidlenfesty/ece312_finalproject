#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#include "lcd.h"
#include "fifo.h"
#include "comms.h"
#include "periph.h"

#define SONG_NAME_LEN 16

/* ---- Globally available flags ---- */
extern bool update_display;

#endif