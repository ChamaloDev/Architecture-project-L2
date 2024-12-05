/*
    In this file are all elements used to convert "assembly language"
    into "machine language".
*/




#include "assembly.h"
#include "sub/read_assembly_file.h"
#include "sub/compile_assembly_code.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int assemble(char *inputPath, char *outputPath) {
    // Load the input file (in reading mode)
    FILE *inputFile = fopen(inputPath, "r");
    // If the input file was not found, end the process
    if (!(inputFile)) {
        printf("ERROR: FILE NOT FOUND\n");
        printf("       Unable to find file \"%s\"\n", inputPath);
        return 0;
    }
    // Read file content
    assemblyLine **assemblyLines = readAssemblyFile(inputFile);
    // Close the input file
    fclose(inputFile);

    // Check if the inputFile was read correctly (if not, assemblyLines = NULL)
    if (!(assemblyLines)) {
        printf("ERROR: ASSEMBLY SYNTAX ERROR\n");
        printf("       Unable to read the assembly file content properly\n");
        return 0;
    }

    // Create the output file (in writing mode)
    FILE *outputFile = fopen(outputPath, "w");
    // If the input file was not created, end the process
    if (!(outputFile)) {
        printf("ERROR: FILE NOT FOUND\n");
        printf("       Unable to create file \"%s\"\n", inputPath);
        return 0;
    }
    // TEMPORARY!
    for (int i = 0; assemblyLines[i]; i++) {
        fprintf(outputFile, "Line number:      %d\n", assemblyLines[i]->number);
        if (assemblyLines[i]->label) fprintf(outputFile, "Line label:       \"%s\"\n", assemblyLines[i]->label);
        else                         fprintf(outputFile, "Line label:\n");
        if (assemblyLines[i]->instruction) fprintf(outputFile, "Line instruction: \"%s\"\n", assemblyLines[i]->instruction);
        else                               fprintf(outputFile, "Line instruction:\n");
        if (!(assemblyLines[i]->data))                    fprintf(outputFile, "Line data:\n");
        else if (isValidNumber(assemblyLines[i]->data)) fprintf(outputFile, "Line data:        %d\n", stringToShort(assemblyLines[i]->data));
        else                                              fprintf(outputFile, "Line data:        \"%s\"\n", assemblyLines[i]->data);
        fprintf(outputFile, "\n");
    }
    /*
        TODO!
        Replace the temporary code used for debbugging by the actual one
    */
    // Close the output file
    fclose(outputFile);

    return 1;
}
