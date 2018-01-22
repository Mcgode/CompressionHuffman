//
// Created by Max Godefroy on 15/01/2018.
//

#ifndef COMPRESSIONHUFFMAN_HUFFMANDECOMPRESS_H
#define COMPRESSIONHUFFMAN_HUFFMANDECOMPRESS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct DecodeBytes {
    unsigned short value;
    struct DecodeBytes *sub_elements;
};
typedef struct DecodeBytes DecodeBytes;

struct UncompressEncodedBytes {
    unsigned char value;
    long code;
};
typedef struct UncompressEncodedBytes UncompressEncodedBytes;

void decompressData(FILE *data, const char filename[]);

#endif //COMPRESSIONHUFFMAN_HUFFMANDECOMPRESS_H
