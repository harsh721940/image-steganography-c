#ifndef DECODE_H
#define DECODE_H

#include "types.h"

// structure for storing the information related to decoding
typedef struct DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char magic_string[10];
    int extn_size;
    char extn[10];
    int secret_data_size;

    /* Output File Info */
    char *output_file_fname;
    FILE *fptr_output_file;
}DecodeInfo;

// Decoding function prototypes

// validating the arguments passed during decoding
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

// perform decoding
Status do_decoding(char *argv[], DecodeInfo *decInfo);

// opening the required files to required modes
Status open_files_dec(DecodeInfo *decInfo);

// decoding the magic string
Status decode_magic_string(int size, DecodeInfo *decInfo);

// decoding data from stego image
Status decode_data_from_image(int size, DecodeInfo *decInfo);

Status decode_data_from_image2(int size, DecodeInfo *decInfo);

Status decode_data_from_image3(char *data, int size, DecodeInfo *decInfo);

// decoding the data byte by byte from lsb bit
Status decode_byte_from_lsb(char *data, char *image_buffer);

// decoding the size of secret_file extension
Status decode_int_from_lsb(DecodeInfo *decInfo);

// decoding the size of secret file data
Status decode_int_from_lsb1(DecodeInfo *decInfo);

// decoding the secret file extension size
Status decode_stego_file_extn_size(DecodeInfo *decInfo);

// decoding the secret file extension
Status decode_stego_file_extn(DecodeInfo *decInfo);

// decoding secret file size
Status decode_stego_file_size(DecodeInfo *decInfo);

// decoding the secret file data
Status decode_stego_file_secret_data(DecodeInfo *decInfo);

#endif 