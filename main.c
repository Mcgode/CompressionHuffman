#include <stdio.h>
#include <math.h>
#include "HuffmanTable.h"
#include "HuffmanCompress.h"
#include "HuffmanDecompress.h"

void countBytesOccurences(int *occurences, FILE *file, int *letter_count);
void getProbabilities(double probabilities[], int total, const int occurences[]);
double entropy(const double probabilities[]);
double averageLength(EncodedBytesArray *encoded_bytes, const double probabilities[]);

int main(int argc, char *argv[]) {

    if (argc < 4) { perror("Not enough arguments were given"); exit(EXIT_FAILURE); }

    if (strlen(argv[1]) == 0) { perror("No filename was given"); exit(EXIT_FAILURE); }
    char *filename = argv[1];

    if (strlen(argv[2]) == 0) { perror("No compressed filename was given"); exit(EXIT_FAILURE); }
    char *compressed_filename = argv[2];

    if (strlen(argv[3]) == 0) { perror("No decompressed filename was given"); exit(EXIT_FAILURE); }
    char *decompressed_filename = argv[3];

    printf("Filename: %s\n", filename);

    FILE *file = fopen(filename, "rb");
    if( file == NULL ) {
        perror(filename);
        return EXIT_FAILURE;
    }

    static int occurences[256];
    int bytes_count = 0;
    countBytesOccurences(occurences, file, &bytes_count);

    static double probabilities[256];
    getProbabilities(probabilities, bytes_count, occurences);

    printf("Entropy H(X) = %f\n", entropy(probabilities));

    HuffmanTable huffman_table = makeHuffmanTable(probabilities);
    printf("HuffmanTable address: %p\n", &huffman_table);
    printf("Table size: %d\n", huffman_table.size);
    int different_letters = huffman_table.size;
    arrangeHuffmanTable(&huffman_table);

    EncodedBytesArray encoded_bytes = encodeHuffmanTable(&huffman_table, different_letters);
    printf("Average length ν = %f\n", averageLength(&encoded_bytes, probabilities));
    for (int i = 0; i < encoded_bytes.size; i++)
    {
        printf("%x: ", encoded_bytes.array[i].value);

        for (int j = 0; j < encoded_bytes.array[i].level; j++)
        {
            printf("%d", (int)(encoded_bytes.array[i].code >> j) % 2);
        }
        printf("\n");
    }

    compressData(file, &encoded_bytes, compressed_filename);

    fclose(file);

    FILE *compressed_file = fopen(compressed_filename, "rb");
    if( compressed_file == NULL ) {
        perror(compressed_filename);
        return EXIT_FAILURE;
    }

    decompressData(compressed_file, decompressed_filename);

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

double entropy(const double probabilities[]) {
    double total = 0;
    for (int i = 0; i < 256; i++) {
        if (probabilities[i] > 0) {
            total += - probabilities[i] * log2(probabilities[i]);
        }
    }
    return total;
}

double averageLength(EncodedBytesArray *encoded_bytes, const double probabilities[]) {
    double total = 0;
    for (int i = 0; i < encoded_bytes->size; i++) {
        double probability = probabilities[encoded_bytes->array[i].value];
        total += probability * (double)(encoded_bytes->array[i].level);
    }
    return total;
}

