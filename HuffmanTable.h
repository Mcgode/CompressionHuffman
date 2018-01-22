//
// Created by Max Godefroy on 12/01/2018.
//

#ifndef COMPRESSIONHUFFMAN_HUFFMANTABLE_H
#define COMPRESSIONHUFFMAN_HUFFMANTABLE_H

#include <stdlib.h>
#include <stdio.h>

struct HuffmanTableElement {
    unsigned short value;
    double probability;
    struct HuffmanTableElement *sub_elements;
};
typedef struct HuffmanTableElement HuffmanTableElement;

struct HuffmanTable {
    HuffmanTableElement *table;
    int size;
};
typedef struct HuffmanTable HuffmanTable;

struct EncodedBytes {
    unsigned char value;
    long code;
    int level;
};
typedef struct EncodedBytes EncodedBytes;

struct EncodedBytesArray {
    EncodedBytes *array;
    int size;
};
typedef struct EncodedBytesArray EncodedBytesArray;


HuffmanTable makeHuffmanTable(const double probabilities[]);
void arrangeHuffmanTable(HuffmanTable *huffman_table);
EncodedBytesArray encodeHuffmanTable(HuffmanTable *table, int size);

#endif //COMPRESSIONHUFFMAN_HUFFMANTABLE_H
