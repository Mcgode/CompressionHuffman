//
// Created by Max Godefroy on 12/01/2018.
//

#include "HuffmanTable.h"


void initHuffmanTableElement(HuffmanTableElement *element)
{
    element->value = 0;
    element->probability = 0;
    element->sub_elements = NULL;
}


void exchange(HuffmanTableElement huffman_table[], int i, int j)
{
    HuffmanTableElement temp = huffman_table[i];
    huffman_table[i] = huffman_table[j];
    huffman_table[j] = temp;
}


void sortHuffmanTable(HuffmanTableElement huffman_table[], int start, int end)
{
    if (start >= end) { return; }

    const double pivot = huffman_table[start].probability;
    int left = start - 1;
    int right = end;

    while(1)
    {
        do right--; while(huffman_table[right].probability > pivot);
        do left++; while(huffman_table[left].probability < pivot);
        if(left < right)
            exchange(huffman_table, left, right);
        else break;
    }
}


void printTable(HuffmanTableElement table[], int size)
{
    for (int i = 0; i < size; i++) {
        printf("%d (%lf), ", table[i].value, table[i].probability);
    }
    printf("end\n");
}


HuffmanTable makeHuffmanTable(const double probabilities[])
{
    int size = 0;

    for (int i = 0; i < 256; i++) {
        if (probabilities[i] > 0) { size++; }
    }

    HuffmanTableElement *huffman_table_array = malloc(sizeof(HuffmanTableElement) * size);

    int current_index = 0;
    for (int i = 0; i < 256; i++) {
        if (probabilities[i] > 0) {

            HuffmanTableElement element;
            initHuffmanTableElement(&element);

            element.probability = probabilities[i];
            element.value = (unsigned char)i;

            huffman_table_array[current_index] = element;
            current_index ++;

            if (current_index == size) { break; }
        }
    }

    HuffmanTable huffman_table;
    huffman_table.size = size;
    huffman_table.table = huffman_table_array;

    return huffman_table;
}


void arrangeHuffmanTable(HuffmanTable *huffman_table)
{
    printTable(huffman_table->table, huffman_table->size);
    sortHuffmanTable(huffman_table->table, 0, huffman_table->size);
    printTable(huffman_table->table, huffman_table->size);

    while (huffman_table->size > 1) {
        HuffmanTableElement regroup;
        initHuffmanTableElement(&regroup);
        regroup.value = 256;
        regroup.probability = huffman_table->table[0].probability + huffman_table->table[1].probability;
        regroup.sub_elements = malloc(2 * sizeof(HuffmanTableElement));
        regroup.sub_elements[0] = huffman_table->table[0];
        regroup.sub_elements[1] = huffman_table->table[1];
        huffman_table->size--;
        HuffmanTableElement *table = malloc(sizeof(HuffmanTableElement) * huffman_table->size);
        table[0] = regroup;
        for (int i = 2; i <= huffman_table->size; i++) {
            table[i - 1] = huffman_table->table[i];
        }
        free(huffman_table->table);
        huffman_table->table = table;
        sortHuffmanTable(huffman_table->table, 0, huffman_table->size);
    }

    printTable(huffman_table->table, huffman_table->size);
}


void recursiveEncode(EncodedBytesArray *array, HuffmanTableElement *element, int *current_index, long current_code, int level)
{
    if (element->value < 256) {
        EncodedBytes character;
        character.value = (unsigned char)(element->value);
        character.code = current_code;
        character.level = level;
        array->array[*current_index] = character;
        //free(element);
        (*current_index)++;
    } else {
        recursiveEncode(array, &(element->sub_elements[0]), current_index, current_code, level+1);
        recursiveEncode(array, &(element->sub_elements[1]), current_index, current_code + (1 << level), level+1);
        free(element->sub_elements);
        //free(element);
    }
}


EncodedBytesArray encodeHuffmanTable(HuffmanTable *table, int size)
{
    EncodedBytesArray array;
    array.size = size;
    array.array = malloc(size * sizeof(EncodedBytes));
    int index = 0;
    recursiveEncode(&array, &(table->table[0]), &index, 0, 0);
    //free(table);
    return array;
}

