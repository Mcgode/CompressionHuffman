//
// Created by Max Godefroy on 15/01/2018.
//

#include "HuffmanCompress.h"

void writeByte(FILE *file, EncodedBytes *encoded_byte, unsigned char *current_write_byte, long *total_bit_size)
{
    long base_byte_index = *total_bit_size % 8;
    for (int i = 0; i < encoded_byte->level; i++) {
        unsigned char bit = (unsigned char)((encoded_byte->code >> i) % 2);
        *current_write_byte += bit << base_byte_index;
        base_byte_index++;
        if (base_byte_index >= 8) {
            fwrite(current_write_byte, sizeof(char), 1, file);
            *current_write_byte = 0;
            base_byte_index = 0;
        }
    }
    *total_bit_size += encoded_byte->level;
}

void compressData(FILE *data, EncodedBytesArray *encoded_bytes, const char filename[])
{
    rewind(data);

    FILE *file = fopen(filename, "wb+");
    EncodedBytes encoded_bytes_array[256];

    unsigned short head_size = (unsigned short)(encoded_bytes->size);
    fwrite(&head_size, sizeof(short), 1, file); // We first write down the head size, for tracking during decompression

    for (int i = 0; i < head_size; i++)
    {
        fwrite(&(encoded_bytes->array[i].value), sizeof(char), 1, file); // First, write down byte value
        fwrite(&(encoded_bytes->array[i].code), sizeof(long), 1, file); // Then, write down code numeric value
        encoded_bytes_array[encoded_bytes->array[i].value] = encoded_bytes->array[i];
    }

    unsigned char current_write_byte = 0;
    long total_bit_size = 0;
    unsigned char current_byte;

    long head_end_index = ftell(file);
    fwrite(&total_bit_size, sizeof(long), 1, file);

    while (1)
    {
        fread(&current_byte, sizeof(char), 1, data);

        if (feof(data)) {
            break;
        }

        writeByte(file, &(encoded_bytes_array[current_byte]), &current_write_byte, &total_bit_size);
    }

    free(encoded_bytes->array);

    fwrite(&current_write_byte, sizeof(char), 1, file);

    printf("\nTotal bit size: %ld, written at index %ld\n", total_bit_size, head_end_index);
    fseek(file, head_end_index, SEEK_SET);
    fwrite(&total_bit_size, sizeof(long), 1, file);
    long bit_size;
    fseek(file, head_end_index, SEEK_SET);
    fread(&bit_size, sizeof(long), 1, file);
    printf("Stored data at index %ld: %ld\n", ftell(file) - sizeof(long), bit_size);

    fclose(file);
}