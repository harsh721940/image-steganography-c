/*  Documentation

Name        : HARSH KADAM
Name of the project : Stegnography
Date        : 27/07/2026
Batch Name  : 26011
*/
#include<stdio.h>
#include"encode.h"
#include"types.h"
#include "decode.h"
#include<string.h>

int main(int argc,char *argv[])
{
    // checking minimum command line arguments
    if(argc < 2)
    {
        printf("Invalid arguments\n");
        printf("for encode -> ./a.out -e beautiful.bmp secret.txt\n");
        printf("for decode -> ./a.out -d stego.bmp\n");
        return e_failure;
    }

    // checking for encoding operation
    if(check_operation_type(argv) == e_encode)
    {
        printf("You choosed encoding\n");

        EncodeInfo encInfo;

        // validating encode arguments
        if(read_and_validate_encode_args(argc, argv, &encInfo) == e_success)
        {
            printf("Read and validate successful\n");

            // calling encoding function
            if(do_encoding(&encInfo) == e_success)
            {
                printf("Encoding successful\n");
                return e_success;
            }
            else
            {
                printf("Encoding unsuccessful\n");
                return e_failure;
            }
        }
        else
        {
            printf("Read and validate unsuccessful\n");
            return e_failure;
        }

        return e_success;
    }

    // checking for decoding operation
    else if(check_operation_type(argv) == e_decode)
    {
        printf("You choosed decoding\n");

        DecodeInfo decInfo;

        // validating decode arguments
        if(read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("Read and validate successful\n");

            // calling decoding function
            if(do_decoding(argv, &decInfo) == e_success)
            {
                printf("Decoding successful\n");
                return e_success;
            }
            else
            {
                printf("Decoding unsuccessful\n");
                return e_failure;
            }
        }
        else
        {
            printf("Read and validate unsuccessful\n");
            return e_failure;
        }
    }

    // invalid operation
    else
    {
        printf("Invalid arguments\n");
        printf("for encode -> ./a.out -e beautiful.bmp secret.txt\n");
        printf("for decode -> ./a.out -d stego.bmp\n");
        return e_failure;
    }
}
OperationType check_operation_type(char *argv[])
{
    // checking for encode option
    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }

    // checking for decode option
    else if(strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }

    // returning unsupported for invalid option
    else
    {
        return e_unsupported;
    }
}