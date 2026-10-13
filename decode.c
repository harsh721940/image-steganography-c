#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"

// validating the arguments passed through command line for decoding
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(argv[2]!=NULL && strstr(argv[2], ".bmp")!=NULL)
    {
        printf(".bmp file present\n");
        decInfo->stego_image_fname=argv[2];
    }

    else
    {
        printf("Not enough arguments\nDestination file name is not present\n");
        return e_failure;
    }

    if(argv[3]!=NULL)
    {
        printf("destination file is present\n");
        char *token=strtok(argv[3],".");
        decInfo->output_file_fname=token;
    }

    else
    {
        decInfo->output_file_fname="output.txt";
    }

    return e_success;
}

// function definition for decoding
Status do_decoding(char *argv[], DecodeInfo *decInfo)
{
    printf("<------ Started Decoding ------->\n");

    // function call for opening the required files for decoding in required mode
    if(open_files_dec(decInfo) == e_success)
    {
        printf("Files opened successfully\n");
    }
    else
    {
        printf("Failed to open files\n");
        return e_failure;
    }

    // skipping the 54 bytes of header
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    char magic[10];
    printf("Enter the magic string: ");
    scanf(" %[^\n]", magic);

    int len = strlen(magic);

    // function call for decoding the magic string
    if(decode_magic_string(len, decInfo) == e_success)
    {
       printf("Magic string decoded successfully\n");
       printf("User magic     : %s\n", magic);
       printf("Decoded magic  : %s\n", decInfo->magic_string);
    }
    else
    {
        printf("Failed to decode magic string\n");
        return e_failure;
    }

    // comparing the magic string whether the user has entered correct magic string or not
    if(strcmp(magic, decInfo->magic_string) != 0)
    {
        printf("Magic string does not match\n");
        return e_failure;
    }

    // function call for decoding the stego file extension size
    if(decode_stego_file_extn_size(decInfo) == e_success)
    {
        printf("File extension size decoded successfully\n");
        printf("File extension size = %d bytes\n", decInfo->extn_size);
    }
    else
    {
        printf("Failed to decode file extension size\n");
        return e_failure;
    }

    // function call for decoding the secret file extension encoded in stego file
    if(decode_stego_file_extn(decInfo) == e_success)
    {
        printf("File extension decoded successfully\n");
        printf("File extension is %s\n", decInfo->extn);
    }
    else
    {
        printf("Failed to decode file extension\n");
        return e_failure;
    }

    if(argv[3] != NULL)
    {
        strcat(decInfo->output_file_fname, decInfo->extn);
        printf("Output file name is %s\n", decInfo->output_file_fname);
    }
    else
    {
        printf("Output file name is %s\n", decInfo->output_file_fname);
    }

    decInfo->fptr_output_file = fopen(decInfo->output_file_fname, "w");

    // function call for decoding the secret file size encoded in the stego file
    if(decode_stego_file_size(decInfo) == e_success)
    {
        printf("Secret file size decoded successfully\n");
        printf("Secret data size is %d\n", decInfo->secret_data_size);
    }
    else
    {
        printf("Failed to decode secret file size\n");
        return e_failure;
    }

    // function call decoding the secret data encoded in the stego file
    if(decode_stego_file_secret_data(decInfo) == e_success)
    {
        printf("Secret data decoded successfully\n");
    }
    else
    {
        printf("Failed to decode secret data\n");
        return e_failure;
    }

    return e_success;
}

// Function definition for opening files for decoding in required modes
Status open_files_dec(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");

    if(decInfo->fptr_stego_image == NULL)
    {
        printf("Failed to open stego image file\n");
        return e_failure;
    }
    else
    {
        printf("Stego image file opened successfully\n");
    }

    return e_success;
}

// function definition for decoding data from the image
Status decode_data_from_image(int size, DecodeInfo *decInfo)
{
   char buffer[8];

   for(int i=0;i<size;i++)
   {
     fread(buffer, 8, 1, decInfo->fptr_stego_image);
     decode_byte_from_lsb(&decInfo->magic_string[i], buffer);
   }

   decInfo->magic_string[size]='\0';

   return e_success;
}

Status decode_data_from_image2(int size, DecodeInfo *decInfo)
{
    char buffer[8];   // Buffer to store 8 bytes read from stego image

    for(int i = 0; i < size; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image); // Read 8 bytes from image
        decode_byte_from_lsb(&decInfo->extn[i], buffer); // Decode one character from LSBs
    }

    decInfo->extn[size] = '\0'; // Null terminate extension string

    return e_success; // Return success status
}

Status decode_data_from_image3(char *data, int size, DecodeInfo *decInfo)
{
    char buffer[8]; // Buffer to hold 8 bytes from image

    for(int i = 0; i < size; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image); // Read encoded bytes
        decode_byte_from_lsb(&data[i], buffer); // Decode one byte from LSBs
        fwrite(&data[i], 1, 1, decInfo->fptr_output_file); // Write decoded byte to output file
    }

    return e_success; // Return success status
}

Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    *data = 0; // Initialize decoded byte

    for(int i = 0; i <= 7; i++)
    {
        *data = *data | ((image_buffer[i] & 1) << (7 - i)); // Extract LSB and build byte
    }

    return e_success; // Return success status
}

Status decode_int_from_lsb(DecodeInfo *decInfo)
{
    char buffer[32]; // Buffer to store 32 bytes from image

    fread(buffer, 32, 1, decInfo->fptr_stego_image); // Read 32 bytes

    decInfo->extn_size = 0; // Initialize extension size

    for(int i = 0; i <= 31; i++)
    {
        decInfo->extn_size = decInfo->extn_size | ((buffer[i] & 1) << (31 - i)); // Decode integer bit by bit
    }

    return e_success; // Return success status
}

Status decode_int_from_lsb1(DecodeInfo *decInfo)
{
    char buffer[32]; // Buffer to hold 32 bytes from image

    fread(buffer, 32, 1, decInfo->fptr_stego_image); // Read encoded integer data

    decInfo->secret_data_size = 0; // Initialize secret data size

    for(int i = 0; i <= 31; i++)
    {
        decInfo->secret_data_size = decInfo->secret_data_size | ((buffer[i] & 1) << (31 - i)); // Extract integer bits
    }

    return e_success; // Return success status
}

// function definition for decoding the magic string
Status decode_magic_string(int size, DecodeInfo *decInfo)
{
    if(decode_data_from_image(size, decInfo)==e_success)
    {
        return e_success;
    }

}

// function definition for decoding the secret file extension size encoded in the stego file
Status decode_stego_file_extn_size(DecodeInfo *decInfo)
{
    if(decode_int_from_lsb(decInfo)==e_success)
    {
        return e_success;
    }

}

// function definition for decoding the secret file extension encoded in the stego file
Status decode_stego_file_extn(DecodeInfo *decInfo)
{
    if(decode_data_from_image2(decInfo->extn_size, decInfo)==e_success)
    {
        return e_success;
    }
}

// function definition for decoding the secret file size encoded in the stego file
Status decode_stego_file_size(DecodeInfo *decInfo)
{
    if(decode_int_from_lsb1(decInfo)==e_success)
    {
        return e_success;
    }
}

// functioin definition for decoding the secret file data encoded in the stego file
Status decode_stego_file_secret_data(DecodeInfo *decInfo)
{
    char buffer[decInfo->secret_data_size];

    if(decode_data_from_image3(buffer, decInfo->secret_data_size, decInfo)==e_success)
    {
        return e_success;
    }
}
