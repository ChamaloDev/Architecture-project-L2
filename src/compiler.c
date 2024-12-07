/*
    In this file are all elements used to convert "assembly language"
    into "machine language".
*/




#include "compiler.h"




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
    assemblyLine **assemblyCode = readAssemblyFile(inputFile);
    // Close the input file
    fclose(inputFile);

    // Check if the inputFile was read correctly (if not, assemblyCode = NULL)
    if (!(assemblyCode)) {
        printf("ERROR: COMPILATION ERROR\n");
        printf("       Unable to compile file \"%s\"\n", inputPath);
        return 0;
    }

    // Compile assembly code
    char *compiledCode = compile(assemblyCode);
    if (!(compiledCode)) {
        printf("ERROR: COMPILATION ERROR\n");
        printf("       Unable to compile file \"%s\"\n", inputPath);
        // Free memory
        for (int i = 0; assemblyCode[i]; i++) {
            free(assemblyCode[i]->label);
            free(assemblyCode[i]->instruction);
            free(assemblyCode[i]->parameter);
            free(assemblyCode[i]);
        }
        free(assemblyCode);
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
    // Write the compiled code into it
    fprintf(outputFile, "%s", compiledCode);
    // Close the output file
    fclose(outputFile);

    // Free memory
    free(compiledCode);
    for (int i = 0; assemblyCode[i]; i++) {
        free(assemblyCode[i]->label);
        free(assemblyCode[i]->instruction);
        free(assemblyCode[i]->parameter);
        free(assemblyCode[i]);
    }
    free(assemblyCode);

    return 1;
}
