#ifndef MAIN_H_
#define MAIN_H_

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include "fat.h"
#include "fat_config.h"
#include "partition.h"
#include "sd_raw.h"
#include "sd_raw_config.h"
#include "fifo.h"
#include "comms.h"
#include "periph.h"
#include "wav.h"

#define MAX_SONG_NUM    9
#define SONG_BUF_LEN    256 // NO MORE THAN 256!!!


// Song playing
extern uint16_t song_buf[2][SONG_BUF_LEN];  //! Buffers to play song from
extern uint8_t song_position;               //! where we are in the buffers
extern bool song_buf_select;                //! which buffer we're using

#endif