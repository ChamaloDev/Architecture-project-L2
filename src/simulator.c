/*
    In this file are all elements used to simulate
    the execution of "machine language".
*/


#include "simulator.h"
#include <stdio.h>
#include <stdlib.h>




short hexaToShort(char *hexa) {
    if (!(hexa)) return 0;
    short value = 0;
    for (int i = 0; hexa[i]; i++) value = 16*value + (('0' <= hexa[i] && hexa[i] <= '9') ? (hexa[i] - '0') : ((hexa[i]-10) - 'a'));
    return value;
}


memoryRegistry *newMemoryRegistry(long long size) {
    memoryRegistry *memo = malloc(sizeof(memoryRegistry));
    memo->registry       = malloc(size * sizeof(short));
    memo->size           = size;
    memo->sp             = 0;
    return memo;
}


instructionLine *readCompiledLine(char *line) {
    // Spliting the line into its 2 elements : instruction code and parameter
    char            *instr  = readWord(&line);
    char            *para   = readWord(&line);
    // Initializing the instructionLine object
    instructionLine *instrL = malloc(sizeof(instructionLine));
    instrL->instruction     = hexaToShort(instr);
    instrL->parameter       = hexaToShort(para);
    return instrL;
}


instructionList *readCompiledFile(char *filePath) {
    // Load the file (in reading mode)
    FILE *file = fopen(filePath, "r");
    // If the file was not found, throw an error
    if (!(file)) {
        printf("ERROR: FILE NOT FOUND\n");
        printf("       Unable to find file \"%s\"\n", filePath);
        return NULL;
    }

    // TODO!
    return NULL;
}


// TODO!
