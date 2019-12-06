#include "fifo.h"

static struct {
    FIFO_TYPE buffer[FIFO_LEN];
    uint16_t rd_ptr;
    uint16_t wr_ptr;
    uint16_t entries;
} fifo;

void fifo_init() {
    fifo.rd_ptr = 0;
    fifo.wr_ptr = 0;
    fifo.entries = 0;
}

fifo_err_t fifo_pop(FIFO_TYPE* dest) {
    // FIFO is empty, nothing to return!
    if (fifo.entries == 0) {
        return FIFO_EMPTY;
    }

    // Entries in FIFO, pop
    *dest = fifo.buffer[fifo.rd_ptr];
    fifo.rd_ptr++;
    fifo.entries--;

    // Ensure pointers don't overflow!
    if (fifo.rd_ptr == FIFO_LEN) {
        fifo.rd_ptr = 0;
    }

    return FIFO_SUCCESS;
}

fifo_err_t fifo_push(FIFO_TYPE src) {
    // FIFO is full, don't store anything
    if (fifo.entries == FIFO_LEN) {
        return FIFO_FULL;
    }

    // FIFO has space, write
    fifo.buffer[fifo.wr_ptr] = src;
    fifo.wr_ptr++;
    fifo.entries++;

    // Ensure pointers don't overflow!
    if (fifo.wr_ptr == FIFO_LEN) {
        fifo.wr_ptr = 0;
    }

    return FIFO_SUCCESS;
}