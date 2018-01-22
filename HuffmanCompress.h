//
// Created by Max Godefroy on 15/01/2018.
//

#ifndef COMPRESSIONHUFFMAN_HUFFMANCOMPRESS_H
#define COMPRESSIONHUFFMAN_HUFFMANCOMPRESS_H

#include "HuffmanTable.h"
#include <stdio.h>

void compressData(FILE *data, EncodedBytesArray *encoded_bytes, const char filename[]);

#endif //COMPRESSIONHUFFMAN_HUFFMANCOMPRESS_H
