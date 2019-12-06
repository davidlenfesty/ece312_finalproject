#include <stdio.h>
#include <stdint.h>
#include <string.h>

/** @brief Struct to contain chunk header information
 */
typedef struct {
    char ckID[4];                      //! Chunk type
    uint32_t cksize;                    //! Size of chunk in bytes
} ck_hdr_t;

/** @brief Struct to contain master WAV chunk info
 */
struct {
    char WAVEID[4];                     //! WAVE ID
} ck_master_t;

/** @brief Types of extension that a fmt type chunk can have
 */
typedef enum {
    CK_FMT_STD = 0U,
    CK_FMT_NOEXT,
    CK_FMT_EXT,
} ck_fmt_ext_t;

/** @brief Struct to hold fmt chunk data
 */
struct {
    struct {
        uint16_t wFormatTag;            //! Type of data format.
        uint16_t nChannels;             //! Number of channels
        uint32_t nSamplesPerSec;        //! Sample rate
        uint32_t nAvgBytesPerSec;       //! Data rate
        uint16_t nBlockAlign;           //! Block size in bytes for data
        uint16_t wBitsPerSample;        //! Bits per sample
        uint16_t cbSize;                //! Size of fmt type extension. Only valid if chunk size is 18 or 40.
        uint16_t wValidBitsPerSample;   //! Number of valid bits per sample. Only valid if chunk size is 40
        uint32_t dwChannelMask;         //! Speaker position mask. Only valid if chunk size is 40
        uint8_t  SubFormat[16];          //! GUID for data format. Only vaid if chunk size is 40
    } data;

    ck_fmt_ext_t type;
} ck_fmt_t;
