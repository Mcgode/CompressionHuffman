#include <stdio.h>
#include "HuffmanTable.h"
#include "HuffmanCompress.h"
#include "HuffmanDecompress.h"

#ifdef _WIN32
    #define FILENAME "C:\\Users\\Max Godefroy\\Developer\\C\\C_Training\\CompressionHuffman\\test.txt"
    #define COMPRESSED_FILENAME "C:\\Users\\Max Godefroy\\Developer\\C\\C_Training\\CompressionHuffman\\test.comp"
    #define DECOMPRESSED_FILENAME "C:\\Users\\Max Godefroy\\Developer\\C\\C_Training\\CompressionHuffman\\test-d.txt"
#elif __APPLE__
    #define FILENAME "/Users/max/Developer/C/C_Training/CompressionHuffman/test.txt"
    #define COMPRESSED_FILENAME "/Users/max/Developer/C/C_Training/CompressionHuffman/test.comp"
    #define DECOMPRESSED_FILENAME "/Users/max/Developer/C/C_Training/CompressionHuffman/test-d.txt"
#endif

void countBytesOccurences(int *occurences, FILE *file, int *letter_count);
void getProbabilities(double probabilities[], int total, const int occurences[]);

int main() {

    FILE *file = fopen(FILENAME, "rb");
    if( file == NULL ) {
        perror(FILENAME);
        return EXIT_FAILURE;
    }

    static int occurences[256];
    int bytes_count = 0;
    countBytesOccurences(occurences, file, &bytes_count);

    static double probabilities[256];
    getProbabilities(probabilities, bytes_count, occurences);

    HuffmanTable huffman_table = makeHuffmanTable(probabilities);
    printf("HuffmanTable address: %p\n", &huffman_table);
    printf("Table size: %d\n", huffman_table.size);
    int different_letters = huffman_table.size;
    arrangeHuffmanTable(&huffman_table);

    EncodedBytesArray encoded_bytes = encodeHuffmanTable(&huffman_table, different_letters);
    for (int i = 0; i < encoded_bytes.size; i++)
    {
        printf("%x: ", encoded_bytes.array[i].value);

        for (int j = 0; j < encoded_bytes.array[i].level; j++)
        {
            printf("%d", (int)(encoded_bytes.array[i].code >> j) % 2);
        }
        printf("\n");
    }

    compressData(file, &encoded_bytes, COMPRESSED_FILENAME);

    fclose(file);

    FILE *compressed_file = fopen(COMPRESSED_FILENAME, "rb");
    if( compressed_file == NULL ) {
        perror(COMPRESSED_FILENAME);
        return EXIT_FAILURE;
    }

    decompressData(compressed_file, DECOMPRESSED_FILENAME);

    fclose(compressed_file);

    return 0;
}


void countBytesOccurences(int *occurences, FILE *file, int *letter_count) {
    printf("Will count characters\n");
    unsigned char character;
    while (1) {
        fread(&character, sizeof(char), 1, file);
        if (feof(file)) { break; }
        occurences[character] ++;
        (*letter_count) ++;
    }
    printf("\n");
    for (int i = 0; i < 256; i++) {
        if (occurences[i] > 0) {
            printf("%c: %d\n", i, occurences[i]);
        }
    }
}


void getProbabilities(double probabilities[], int total, const int occurences[]) {
    for (int i = 0; i < 256; i++) {
        probabilities[i] = (double)occurences[i] / (double)total;
    }
}

