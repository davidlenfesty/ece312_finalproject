#include "main.h"

/* ---- Global Flags ---- */
int8_t display_song = -1;          //! ID of the song currently being displayed, -1 if song list is not initialised
int8_t last_displayed_song = -1;
int8_t selected_song = -1;         //! ID of the selected song. -1 if no song is selected.
bool update_display = false;        //! flag on whether or not to update display

struct {
    char name[SONG_NAME_LEN + 1];   //! array of all song names, names are allocated on heap
    uint8_t num_songs;              //! Number of songs loaded into display memory
    bool playing;                   //! Whether the currently selected song is playing
} songs;


/** @brief Handles right button press
 */
void handle_left_press() {
    if (songs.num_songs == 0) {
        lcd_clrscr();
        lcd_puts("No songs to play!");
    } else {
        // Choose lower ID song. Wrap around.
        display_song--;
        if (display_song == 0) {
            display_song = songs.num_songs - 1;
        }
    }
}

/** @brief Handles left button press
 */
void handle_right_press() {
    if (songs.num_songs == 0) {
        lcd_clrscr();
        lcd_puts("No songs to play!");
    } else {
        // Choose higher ID song. Wrap around.
        display_song++;
        if (display_song == songs.num_songs) {
            display_song = 0;
        }
    }
}

/** @brief Handles playpause button press.
 * 
 * Based on selected song and displayed song, will select new song
 * or play/pause song.
 */
void handle_playpause_press() {
    // If there are no songs displayed, error
    if (display_song == -1) {
        lcd_clrscr();
        lcd_puts("No songs to play!");
        return;
    }

    // If no song selected, select the correct one.
    if (selected_song == -1) {
        comms_select_file(display_song);
        return;
    }

    // Song selected, send play/pause
    if (songs.playing) {
        comms_pause();
        songs.playing = false;
    } else {
        comms_play();
        songs.playing = true;
    }
    
    return;
}

int main() {
    // Scratch variables
    uint8_t incoming_cmd = 0;

    fifo_init();
    gpio_init();
    usart_init();

    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    lcd_gotoxy(0,0);

    // Main loop
    while (1) {
        // Handle incoming messages
        if (fifo_pop(&incoming_cmd) == FIFO_SUCCESS) {
            switch (incoming_cmd) {
                // Clear songs, so there are none to display
                case (COMMS_CMD_CLR):
                    songs.num_songs = 0;
                    display_song = -1;
                    break;
                // change number of songs
                case (COMMS_CMD_NUM):
                    while(fifo_pop((FIFO_TYPE*) &songs.num_songs) != FIFO_SUCCESS);
                    break;
                default:
                    lcd_clrscr();
                    lcd_puts("Incorrect command!");
                    break;
            }
            
        }

        // Handle "left" button press
        if (BUTTONS_PORT & (1 << BUTTONS_LEFT_PIN)) {
            handle_left_press();
            _delay_ms(100); // Delay to debounce
        }

        // Handle "right" button press
        if (BUTTONS_PORT & (1 << BUTTONS_LEFT_PIN)) {
            handle_left_press();
            _delay_ms(100); // Delay to debounce
        }

        // Handle play/pause button press
        if (BUTTONS_PORT & (1 << BUTTONS_LEFT_PIN)) {
            handle_left_press();
            _delay_ms(100); // Delay to debounce
        }

        // Update display periodically
        if (update_display) {
            // check if song has changed
            if (last_displayed_song != display_song) {
                comms_query_name(display_song);

                char tmp_chr;

                // Wait for command to come in
                while (fifo_pop((FIFO_TYPE*) &tmp_chr) != FIFO_SUCCESS);

                // TODO add error handling here

                // Wait for length info to come in
                while (fifo_pop((FIFO_TYPE*) &tmp_chr) != FIFO_SUCCESS);
                uint8_t len = tmp_chr;

                // Read in string
                for (uint8_t i = 0; i < len; i++) {
                    // Hold until fifo returns successfully
                    while(fifo_pop((FIFO_TYPE*) &songs.name[i]) != FIFO_SUCCESS);
                }
            }
            lcd_clrscr();

            if (display_song != -1) {
                lcd_puts(songs.name);
                lcd_gotoxy(0, 1);
                lcd_putc('1' + display_song);
                lcd_gotoxy(13,1);
                lcd_putc('1' + display_song);
                lcd_putc('/');
                lcd_putc('0' + songs.num_songs);

            } else {
                lcd_puts("No song!");
            }

            update_display = false;
            last_displayed_song = display_song;
        }
    }

}