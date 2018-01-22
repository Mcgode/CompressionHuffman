//
// Created by Max Godefroy on 15/01/2018.
//

#include "HuffmanDecompress.h"


struct DecompressThing {
    short *sub_elements;
    struct DecompressThing *children;
    short size;
};
typedef struct DecompressThing DecompressThing;

UncompressEncodedBytes *getHeadData(FILE *data, short *head_size)
{
    fread(head_size, sizeof(short), 1, data);

    UncompressEncodedBytes *head_data = malloc(sizeof(UncompressEncodedBytes) * *head_size);

    for (int i = 0; i < *head_size; i++) {
        UncompressEncodedBytes encoded_bytes;
        fread(&encoded_bytes.value, sizeof(char), 1, data);
        fread(&encoded_bytes.code, sizeof(long), 1, data);
        head_data[i] = encoded_bytes;
    }

    return head_data;
}


void makeDecodeThing(DecompressThing *decompress_thing, const UncompressEncodedBytes *head_data, long level)
{
    if (decompress_thing->size == 1)
        return;
    else if (decompress_thing->size > 1)
    {

        decompress_thing->children = malloc(sizeof(DecompressThing) * 2);
        short amount_0 = 0;
        short amount_1 = 0;

        for (short i = 0; i < decompress_thing->size; i++)
        {
            char bit = (char)((head_data[decompress_thing->sub_elements[i]].code >> level) % 2);

            if (bit == 0) {
                amount_0++;
            } else {
                amount_1++;
            }
        }

        DecompressThing decompress_thing_0;
        DecompressThing decompress_thing_1;

        decompress_thing_0.sub_elements = malloc(sizeof(short) * amount_0);
        decompress_thing_0.size = amount_0;

        decompress_thing_1.sub_elements = malloc(sizeof(short) * amount_1);
        decompress_thing_1.size = amount_1;

        int k = 0;
        int l = 0;

        for (short i = 0; i < decompress_thing->size; i++)
        {
            char bit = (char)((head_data[decompress_thing->sub_elements[i]].code >> level) % 2);

            if (bit == 0) {
                decompress_thing_0.sub_elements[k] = decompress_thing->sub_elements[i];
                k ++;
            } else {
                decompress_thing_1.sub_elements[l] = decompress_thing->sub_elements[i];
                l ++;
            }
        }

        decompress_thing->children[0] = decompress_thing_0;
        decompress_thing->children[1] = decompress_thing_1;

        makeDecodeThing(&decompress_thing->children[0], head_data, level + 1);
        makeDecodeThing(&decompress_thing->children[1], head_data, level + 1);
    }
    else
    {
        perror("decompress_thing->size < 1");
        exit(EXIT_FAILURE);
    }
}


void freeDecompressThing(DecompressThing *thing)
{
    free(thing->sub_elements);
    if (thing->size > 1) {
        freeDecompressThing(&thing->children[0]);
        freeDecompressThing(&thing->children[1]);
        free(thing->children);
    }
}

void deductDecodeBytesRecursive(DecodeBytes *decode, const UncompressEncodedBytes *head_data, DecompressThing *thing)
{
    if (thing->size == 1) {
        decode->value = head_data[thing->sub_elements[0]].value;
    } else {
        decode->value = 256;
        DecodeBytes child_0; DecodeBytes child_1;
        decode->sub_elements = malloc(sizeof(DecodeBytes) * 2);
        decode->sub_elements[0] = child_0;
        decode->sub_elements[1] = child_1;
        deductDecodeBytesRecursive(&decode->sub_elements[0], head_data, &thing->children[0]);
        deductDecodeBytesRecursive(&decode->sub_elements[1], head_data, &thing->children[1]);
    }
}


void deductDecodeBytes(DecodeBytes *head_decode, const short head_size, const UncompressEncodedBytes *head_data)
{
    DecompressThing decompress_thing;
    decompress_thing.sub_elements = malloc(sizeof(short) * head_size);
    decompress_thing.size = head_size;
    for (short i = 0; i < head_size; i++) { decompress_thing.sub_elements[i] = i; }

    printf("Will start deducting DecompressThing objects\n");
    makeDecodeThing(&decompress_thing, head_data, 0);

    printf("Will start deducting DecodeBytes objects\n");
    deductDecodeBytesRecursive(head_decode, head_data, &decompress_thing);

    printf("Will free DecompressingThing objects\n");
    freeDecompressThing(&decompress_thing);
}


void printDecodeRecursive(DecodeBytes *decode, char *string, long level) {
    if (decode->value == 256) {
        char *string_0 = malloc(sizeof(char) * (level + 1));
        memset(string_0, '\0', sizeof(char) * (level + 1));
        strcpy(string_0, string);

        char *string_1 = malloc(sizeof(char) * (level + 1));
        memset(string_1, '\0', sizeof(char) * (level + 1));
        strcpy(string_1, string);

        strcat(string_0, "0"); strcat(string_1, "1");

        printDecodeRecursive(&decode->sub_elements[0], string_0, level+1);
        printDecodeRecursive(&decode->sub_elements[1], string_1, level+1);

        free(string_0); free(string_1);
    } else {
        printf("%x: %s\n", decode->value, string);
    }
}


void printDecode(DecodeBytes *decode) {
    char string[] = "";
    printDecodeRecursive(decode, string, 0);
}


char readNextBit(FILE *data, unsigned char *current_byte, long *current_bit, const long max_bit)
{
    if (*current_bit >= max_bit)
        return 0;
    char index = (char)(*current_bit % 8);
    char bit = (char)((*current_byte >> index) % 2);
    if (index + 1 >= 8) {
        fread(current_byte, sizeof(char), 1, data);
    }
    (*current_bit)++;
    return bit;
}


void decompressData(FILE *data, const char filename[])
{
    rewind(data);

    FILE *file = fopen(filename, "wb");
    short head_size = 0;
    long total_bit_count;

    UncompressEncodedBytes *head_data = getHeadData(data, &head_size);

    printf("Now at index %ld\n", ftell(data));

    for (int i = 0; i < head_size; i++) {
        printf("%x: %ld\n", head_data[i].value, head_data[i].code);
    }

    DecodeBytes head_decode;
    deductDecodeBytes(&head_decode, head_size, head_data);

    fread(&total_bit_count, sizeof(long), 1, data);
    printf("Bit count: %ld\n", total_bit_count);

    printf("Now at index %ld\n", ftell(data));

    unsigned char current_byte;
    long bit_index = 0;
    fread(&current_byte, sizeof(char), 1, data);

    printDecode(&head_decode);

    while (bit_index < total_bit_count) {
        DecodeBytes *decode = &head_decode;
        while (decode->value == 256) {
            char next_bit = readNextBit(data, &current_byte, &bit_index, total_bit_count);
            decode = &decode->sub_elements[next_bit];
        }
        fwrite(&decode->value, sizeof(char), 1, file);
    }

    printf("\nBit index: %ld vs %ld total bits\n", bit_index, total_bit_count);

    fclose(file);
}
