#ifndef FIFO_H_
#define FIFO_H_

#include <stdint.h>

typedef enum {
    FIFO_SUCCESS,
    FIFO_FULL,
    FIFO_EMPTY
} fifo_err_t;

#define FIFO_LEN 20
#define FIFO_TYPE uint8_t

/** @brief Initialises FIFO
 * 
 * This MUST be called before any other fifo_* functions are called.
 * Unless memory is initialised to 0. Then it's fine I guess.
 */
void fifo_init();

/** @brief Pops most recent addition off of FIFO buffer.
 * 
 * @retval FIFO_SUCCESS Successfully read data from FIFO
 * @retval FIFO_EMPTY Buffer was empty, no data read
 */
fifo_err_t fifo_pop(FIFO_TYPE* dest);

/** @brief Pushes entry onto FIFO buffer.
 * 
 * @retval FIFO_SUCCESS successfully wrote data to FIFO buffer
 * @retval FIFO_FULL buffer, was full, no data written
 */
fifo_err_t fifo_push(FIFO_TYPE src);

#endif