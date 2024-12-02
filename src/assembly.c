/*
    In this file are all elements used to convert
    "assembly language" into "machine language".
*/


#include "assembly.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Read a line of assembly code
assemblyLine *readAssemblyLine(short number, char *line) {
    assemblyLine *l = malloc(sizeof(assemblyLine));
    l->number       = number;
    // TODO!
    return l;
};

// Create a new memory space
memoryRegistry *newMemoryRegistry() {
    memoryRegistry *m   = malloc(sizeof(memoryRegistry));
    m->registry         = malloc(sizeof(short) * 5000);
    m->sp               = 0;
    return m;
};

// Assemble "assembly language" into "machine language"
// Return 1 if successful, 0 otherwise
int assemble(char *inputPath, char *outputPath) {
    // Load the input file (in reading mode)
    FILE *inputFile = fopen(inputPath, "r");
    // If the input file was not found, end the process
    if (!(inputFile)) {
        printf("ERROR: File \"%s\" was not found\n", inputPath);
        return 0;
    }
    // If the input file was found, continue
    // Create the output file (in writing mode)
    FILE *outputFile = fopen(outputPath, "w");
    return 1;
}
