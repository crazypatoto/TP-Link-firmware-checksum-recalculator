#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "md5.h"

static const uint8_t md5_salt[16] = {
    0x7a,
    0x2b,
    0x15,
    0xed,
    0x9b,
    0x98,
    0x59,
    0x6d,
    0xe5,
    0x04,
    0xab,
    0x44,
    0xac,
    0x2a,
    0x9f,
    0x4e,
};

/** Generates and writes the image MD5 checksum */
static void put_md5(uint8_t *md5, uint8_t *buffer, unsigned int len)
{
    MD5_CTX ctx;

    MD5_Init(&ctx);
    MD5_Update(&ctx, md5_salt, (unsigned int)sizeof(md5_salt));
    MD5_Update(&ctx, buffer, len);
    MD5_Final(md5, &ctx);
}

/** Stores a uint32 as big endian */
static inline void put32(uint8_t *buf, uint32_t val)
{
    buf[0] = val >> 24;
    buf[1] = val >> 16;
    buf[2] = val >> 8;
    buf[3] = val;
}

int main(int argc, char **argv)
{
    FILE *file;
    FILE *write_ptr;    
    uint8_t *image_buffer;
    size_t img_size;

    if (argc != 2)
    {
        printf("Usage: ./checksum [firmware bin file]");
        return 1;
    }

    file = fopen(argv[1], "rb"); // r for read, b for binary
    if (NULL == file)
    {
        printf("Cannot open %s\n", argv[1]);
        return 1;
    }

    fseek(file, 0, SEEK_END); // seek to end of file
    img_size = ftell(file);   // get current file pointer
    fseek(file, 0, SEEK_SET); // seek back to beginning of file
    printf("File size = 0x%x (%ld)\n", img_size, img_size);
    image_buffer = (uint8_t *)malloc(img_size);
    fread(image_buffer, img_size, 1, file);
    fclose(file);

    put32(image_buffer, img_size);
    put_md5(image_buffer + 0x04, image_buffer + 0x14, img_size - 0x14);

    printf("Checksum = ");
    for (size_t i = 0; i < 16; i++)
    {
        printf("%02x", (image_buffer+0x04)[i]);
    }
    printf("\n");

    

    write_ptr = fopen("output.bin", "wb"); // w for write, b for binary
    fwrite(image_buffer, img_size, 1, write_ptr); // write 10 bytes from our buffer
    fclose(write_ptr);

    free(image_buffer);

    return 0;
}