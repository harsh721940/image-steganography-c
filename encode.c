#include<stdio.h>
#include<string.h>
#include"encode.h"
#include"types.h"

Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
    // checking minimum arguments required for encoding
    if(argc < 4)
    {
        printf("Not enough arguments\n");
        printf("for encoding the message give-> ./a.out -e beautiful.bmp secret.txt \n");
        return e_failure;
    }

    // validating source bmp image file
    if(strstr(argv[2], ".bmp") != NULL)
    {
        printf("BMP file is present\n");
        encInfo->src_image_fname = argv[2];
    }
    else
    {
       printf("BMP file is not present\n");
       return e_failure;
    }

    // validating secret text file
    if(strstr(argv[3], ".txt") != NULL)
    {
        printf("Text file is present\n");
        encInfo->secret_fname = argv[3];
    }
    else
    {
        printf("Text file is not present\n");
        return e_failure;
    }

    // checking optional output stego image file
    if(argc >= 5 && strstr(argv[4], ".bmp") != NULL)
    {
        printf("Stego image file is present\n");
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }

    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
    printf("<------- Started Encoding ------->\n");

    // opening all required files
    if(open_files(encInfo) == e_success)
    {
        printf("Files opened successfully\n");
    }
    else
    {
        printf("Failed to open files\n");
        return e_failure;
    }

    // reading magic string from user
    printf("Enter magic string: ");
    scanf("%[^\n]", encInfo->magic);

    // checking image capacity
    if(check_capacity(encInfo) == e_success)
    {
       printf("Capacity checked successfully\n");
    }
    else
    {
        printf("Source image capacity is not sufficient\n");
        return e_failure;
    }

    // copying bmp header
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("Header copied successfully\n");
    }
    else
    {
        printf("Failed to copy header\n");
        return e_failure;
    }

    // encoding magic string
    if(encode_magic_string(encInfo->magic, encInfo) == e_success)
    {
        printf("Magic string encoded successfully\n");
    }
    else
    {
        printf("Failed to encode magic string\n");
    }

    // encoding secret file extension size
    if(encode_secret_file_extn_size(strlen(strchr(encInfo->secret_fname, '.')), encInfo) == e_success)
    {
        printf("Extension size encoded successfully\n");
    }

    // encoding secret file extension
    if(encode_secret_file_extn(strchr(encInfo->secret_fname, '.'), encInfo) == e_success)
    {
        printf("Secret file extension encoded successfully\n");
    }
    else
    {
        printf("Failed to encode secret file extension\n");
        return e_failure;
    }

    // encoding secret file size
    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
    {
        printf("Secret file size encoded successfully\n");
    }
    else
    {
         printf("Failed to encode secret file size\n");
    }

    // encoding secret file data
    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("Secret data encoded successfully\n");
    }
    else
    {
        printf("Failed to encode secret data\n");
    }

    // copying remaining image data
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("Remaining image data copied successfully\n");
    }
    else
    {
        printf("Failed to copy remaining image data\n");
        return e_failure;
    }

    return e_success;
}
Status open_files(EncodeInfo *encInfo)
{
   // opening source bmp image file
   encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");

   if(encInfo->fptr_src_image == NULL)
   {
      printf("Source file is not present\n");
      return e_failure;
   }
   else
   {
      printf("Source file opened successfully\n");
   }

   // opening secret file
   encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");

   if(encInfo->fptr_secret == NULL)
   {
      printf("Secret file is not present\n");
      return e_failure;
   }
   else
   {
      printf("Secret file opened successfully\n");
   }

   // opening output stego image file
   encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");

   if(encInfo->fptr_stego_image == NULL)
   {
      printf("Stego file is not present\n");
      return e_failure;
   }
   else
   {
      printf("Stego file opened successfully\n");
   }

   return e_success;
}


Status check_capacity(EncodeInfo *encInfo)
{
    // getting source image capacity
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    // getting secret file size
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    // checking whether image has enough capacity
    if(encInfo->image_capacity > (strlen(encInfo->magic) * 8 + 32 + 32 + 32 + (encInfo->size_secret_file) * 8))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
uint get_image_size_for_bmp(FILE *fptr_image)
{
    int wid, len;

    // moving file pointer to width position in bmp header
    fseek(fptr_image, 18, SEEK_SET);

    // reading width and height of image
    fread(&wid, 4, 1, fptr_image);
    fread(&len, 4, 1, fptr_image);

    // returning total image capacity
    return wid * len * 3;
}


uint get_file_size(FILE *fptr)
{
    // moving file pointer to end of file
    fseek(fptr, 0, SEEK_END);

    // getting file size
    uint size = ftell(fptr);

    // resetting file pointer to beginning
    rewind(fptr);

    return size;
}


Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    // resetting source image pointer
    rewind(fptr_src_image);

    char buffer[54];

    // reading bmp header
    fread(buffer, 54, 1, fptr_src_image);

    // writing bmp header into destination image
    fwrite(buffer, 1, 54, fptr_dest_image);

    return e_success;
}


Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    // encoding magic string into image
    if(encode_data_to_image(magic_string, strlen(magic_string), encInfo) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}


Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    char buffer[8];

    // encoding each byte of data into image
    for(int i = 0; i < size; i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(data[i], buffer);

        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // encoding one byte into 8 bytes of image data
    for(int i = 7; i >= 0; i--)
    {
        image_buffer[7 - i] = ((data & (1 << i)) >> i) | (image_buffer[7 - i] & 0xfe);
    }

    return e_success;
}


Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    // encoding secret file extension
    if(encode_data_to_image(file_extn, strlen(file_extn), encInfo) == e_success)
    {
        return e_success;
    }
}


Status encode_secret_file_extn_size(int data, EncodeInfo *encInfo)
{
    // encoding extension size
    if(encode_int_to_lsb(data, encInfo) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}


Status encode_int_to_lsb(int data, EncodeInfo *encInfo)
{
    char buffer[32];

    // reading 32 bytes from source image
    fread(buffer, 32, 1, encInfo->fptr_src_image);

    // encoding integer bit by bit
    for(int i = 31; i >= 0; i--)
    {
        buffer[31 - i] = ((data & (1 << i)) >> i) | (buffer[31 - i] & 0xfe);
    }

    // writing modified data into stego image
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}


Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    // encoding secret file size
    if(encode_int_to_lsb(file_size, encInfo) == e_success)
    {
        return e_success;
    }
}


Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buffer[encInfo->size_secret_file];

    // reading secret file data
    fread(buffer, encInfo->size_secret_file, 1, encInfo->fptr_secret);

    // encoding secret file data into image
    return encode_data_to_image(buffer, encInfo->size_secret_file, encInfo);
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    // copying remaining image data
    while(fread(&ch, 1, 1, fptr_src))
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }

    return e_success;
}