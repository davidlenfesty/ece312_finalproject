
/*
 * Copyright (c) 2006-2012 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "main.h"

#define DEBUG 1

const char* DATA_CHUNK_ID = "data";

/* ---- Control Flow Variables ---- */
// Commands
uint8_t cmd_depth = 0;
uint8_t cmd_state;

// Song playing
// TODO fix this atrocious naming
uint16_t song_buf[2][SONG_BUF_LEN]; //! Buffers to play song from
uint8_t song_position;              //! where we are in the buffers
bool song_buf_select;               //! which buffer we're using
bool song_selected;                 //! Flag to say we just selected the song, read data in
int8_t song_selection = -1;         //! ID of selected song
uint32_t song_len;
uint32_t song_read_position;


typedef struct {
    struct fat_file_struct* fd;
    char* name;
} song_info_t;

song_info_t songs[MAX_SONG_NUM];

int main()
{
    gpio_init();
    timer_init();
    usart_init();

    // TODO set SPI to highest freq possible, might just need tweaking
    sd_raw_init();
    /* we will just use ordinary idle mode */
    //set_sleep_mode(SLEEP_MODE_IDLE);

    /* open first partition */
    struct partition_struct* partition = partition_open(sd_raw_read,
                                                        sd_raw_read_interval,
                                                        0,
                                                        0,
                                                        0
                                                        );

    if(!partition)
    {
        /* If the partition did not open, assume the storage device
            * is a "superfloppy", i.e. has no MBR.
            */
        partition = partition_open(sd_raw_read,
                                    sd_raw_read_interval,
                                    0,
                                    0,
                                    -1
                                    );
    }

    /* open file system */
    struct fat_fs_struct* fs = fat_open(partition);

    /* open root directory */
    struct fat_dir_entry_struct directory;
    fat_get_dir_entry_of_path(fs, "/", &directory);

    struct fat_dir_struct* dd = fat_open_dir(fs, &directory);

    // Get all of the song names
    struct fat_dir_entry_struct dir_entry;
    uint8_t song_id = 0;
    while(fat_read_dir(dd, &dir_entry)) {
        // too many files!
        if (song_id == MAX_SONG_NUM) {
            break;
        }

        // Is file, so add to list
        if (!(dir_entry.attributes & FAT_ATTRIB_DIR)) {
            // TODO add checking if its .wav

            // Copy song name into memory
            uint8_t len = strlen(dir_entry.long_name);
            songs[song_id].name = malloc(len);
            strcpy(songs[song_id].name, dir_entry.long_name);

            // Open file and keep info in songs
            fat_open_file(fs, (const struct fat_dir_entry_struct*) songs[song_id].fd);

            // Move on to reading next song
            song_id++;
        }

    }
    
    // Print out all files/directories in top level
    while (1) {
        /// do stuff

        // Handle incoming commands
        if (cmd_depth == 0) {
            if (fifo_pop(&cmd_state) == FIFO_SUCCESS) {
                // Handle incoming command
                switch(cmd_state) {
                    case (COMMS_CMD_PAUSE):
                        // Turn off interrupts for play timer to stop playing
                        TIFR0 &= ~(1 << OCF0A);
                        cmd_depth = 0;
                        break;
                    case (COMMS_CMD_PLAY):
                        // Turn on interrupts for play timer to start playing
                        TIFR0 |= (1 << OCF0A);
                        cmd_depth = 0;
                        break;
                    case (COMMS_CMD_SELECT_FILE):
                        // stop playing whatever song we have right now
                        // Wait for next loop to handle file
                        TIFR0 &= ~(1 << OCF0A);
                        cmd_depth = 1;
                        break;
                    case (COMMS_CMD_QUERY_NAME):
                        // Wait for next loop to handle file info
                        cmd_depth = 1;
                        break;
                }
            }

        }

        if (cmd_depth == 1) {
            uint8_t song_id;
            if (fifo_pop(&song_id) == FIFO_SUCCESS) {
                switch (cmd_state) {
                    case (COMMS_CMD_SELECT_FILE):
                        // select proper song and begin reading in
                        song_selection = song_id;
                        song_selected = 1;
                        break;
                    case (COMMS_CMD_QUERY_NAME):
                        // Just going to hope we can bump the UART baudrate up high enough
                        comms_reply_name(songs[song_id].name);
                        break;
                }
            }
        }

        // TODO check here if we are running into the end of our song buffer, and read in song data
        if (song_position > 128 || song_selected) {
            // Select opposite buffer
            bool read_song_buf = !song_buf_select;

            // Need to get past the metadata
            if (song_selected) {
                // Seek beginning of file
                fat_seek_file(songs[song_selection].fd, 0, FAT_SEEK_SET);

                // Iterate until we get the data
                bool is_data = false;
                ck_hdr_t header;
                while (!is_data) {
                    fat_read_file(songs[song_selection].fd,
                                  (uint8_t* ) &header, sizeof(ck_hdr_t));
                    
                    // Ghetto strcmp
                    for (uint8_t i = 0; i < 4; i++) {
                        if (DATA_CHUNK_ID[i] == header.ckID[i]) {
                            is_data = true;
                        } else {
                            break;
                        }
                    }

                    // If this is metadata, skip the chunk
                    int32_t* offset = NULL;
                    *offset = header.cksize;
                    if (!is_data) {
                        fat_seek_file(songs[song_selection].fd,
                                      offset, FAT_SEEK_CUR);
                    }
                }

                // We are now at the actual data
                // divide by two to get uint16
                song_len = header.cksize / 2;
                song_read_position = 0;
            }

            // hit end of song, no more to read
            if (song_read_position == song_len) {
                song_read_position = 0;
                TIFR0 &= ~(1 << OCF0A);
                return 0;
            }
            
            
            // Read in file
            // 512 bytes to read, 256 samples
            fat_read_file(songs[song_selection].fd, (uint8_t*) song_buf[read_song_buf], 512);
            song_read_position += 256;

            // If we selected the song, make sure we start playing from the right buffer
            if (song_selected) {
                song_buf_select = read_song_buf;
            }

            // Ensure we don't "re-select" the song
            song_selected = false;
        }
    }
    
    return 0;
}