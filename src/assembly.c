/*
    In this file are all elements used to convert
    "assembly language" into "machine language".
*/




#include "assembly.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>




int isStringANumber(char *str) {
    // Missing string are not numbers
    if (!(str)) return 0;
    // If this is a signed number, remove the sign
    if (str[0] == '+' || str[0] == '-') str++;
    int i = 0;
    for (; '0' <= str[i] && str[i] <= '9'; i++);
    // There must be at least 1 number, and the last character reached should be '\0'
    return (i && !(str[i]));
}


short stringToShort(char *str) {
    // If this is a signed number
    int sign = (str[0] == '-') ? -1 : +1;
    if (str[0] == '+' || str[0] == '-') str++;
    // Read the number
    short value = 0;
    char c;
    while ((c = (str++)[0])) value = (10*value) + (c-'0');
    return sign * value;
}


int isBlankString(char *str) {
    while (str[0]) if (!(isspace((str++)[0]))) return 0;
    return 1;
}


assemblyLine *readAssemblyLine(short number, char lines[MAX_ASSEMBLY_LINES][ASSEMBLY_LINE_MAX_SIZE]) {
    assemblyLine *l = malloc(sizeof(assemblyLine));
    l->number       = number+1;
    l->label        = NULL;
    l->instruction  = NULL;
    l->data         = 0;

    // Reading the assembly line
    // To do so, it is splited into a array of string, ignores whitespaces
    char *(tmpArray[3]) = {NULL, NULL, NULL};
    int i = 0, j = 0, k = 0;
    // While the line has not been fully read (including the '\0' character)
    while (j == 0 || lines[number][j-1]) {
        // When a whitespace (here, only '\0' and ' ') character is found
        if (isspace(lines[number][j])) {
            // If the word isn't empty
            if (j != k) {
                // tmpArray cannot contain more than 3 strings, otherwise abort process
                if (i >= 3) {
                    printf("ERROR: INVALID ASSEMBLY LINE FORMAT\n");
                    printf("       Too many elements on line %d\n", number+1);
                    for (int v = 0; v < 3; v++) free(tmpArray[v]);
                    return NULL;
                }
                // If there is space remaining inside tmpArray
                tmpArray[i] = malloc((j-k+1) * sizeof(char));
                for (int v = 0; v < j-k; v++) tmpArray[i][v] = lines[number][v+k];
                tmpArray[i][j-k] = '\0';
                i++;
            }
            k = j+1;
        }
        j++;
    }

    // At least 1 word has to be read
    if (i == 0) {
        printf("ERROR: INVALID ASSEMBLY LINE FORMAT\n");
        printf("       Line %d is empty\n", number+1);
        return NULL;
    }
    // 1 word was read (only an instruction)
    else if (i == 1) {
        l->instruction = tmpArray[0];
    }
    // 2 words were read (label, instruction OR instruction, data)
    else if (i == 2) {
        /*
            TODO!
            Add a check to see if the label only contains 'A-Z', 'a-z', '0-9' and '_'
            First character should be in 'A-Z' or 'a-z'
        */
        // label, instruction
        if (tmpArray[0][strlen(tmpArray[0]) - 1] == ':') {
            tmpArray[0][strlen(tmpArray[0]) - 1] = '\0';
            l->label        = tmpArray[0];
            l->instruction  = tmpArray[1];
        }
        // instruction, data
        else {
            // Check if the string representing the data is a number, if not abort process
            if (!(isStringANumber(tmpArray[1]))) {
                printf("ERROR: INVALID NUMBER\n");
                printf("       String \"%s\" cannot be interpreted as a number\n", tmpArray[1]);
                for (int v = 0; v < 2; v++) free(tmpArray[v]);
                return NULL;
            }
            l->instruction  = tmpArray[0];
            l->data         = stringToShort(tmpArray[1]);
        }
    }
    // 3 words were read (label, instruction, data)
    else if (i == 3) {
        /*
            TODO!
            Add a check to see if the label only contains 'A-Z', 'a-z', '0-9' and '_'
            First character should be in 'A-Z' or 'a-z'
        */
        // Check if the string representing the label ends with ':', if not abort process
        if (tmpArray[0][strlen(tmpArray[0]) - 1] != ':') {
            printf("ERROR: INVALID LABEL\n");
            printf("       String \"%s\" must end with ':' to be read as a label\n", tmpArray[2]);
            for (int v = 0; v < 3; v++) free(tmpArray[v]);
            return NULL;
        }
        // Check if the string representing the data is a number, if not abort process
        if (!(isStringANumber(tmpArray[2]))) {
            printf("ERROR: INVALID NUMBER\n");
            printf("       String \"%s\" cannot be interpreted as a number\n", tmpArray[2]);
            for (int v = 0; v < 3; v++) free(tmpArray[v]);
            return NULL;
        }
        tmpArray[0][strlen(tmpArray[0]) - 1] = '\0';
        l->label        = tmpArray[0];
        l->instruction  = tmpArray[1];
        l->data         = stringToShort(tmpArray[2]);
    }

    return l;
}


assemblyLine **readAssemblyFile(FILE *f) {
    // If no input file is given, abort the process
    if (!(f)) {
        printf("ERROR: MISSING FILE\n");
        printf("       No input file was given\n");
        return NULL;
    }

    // Read file content
    char lines[MAX_ASSEMBLY_LINES][ASSEMBLY_LINE_MAX_SIZE];
    int nbLines = 0;
    rewind(f);  // Set cursor at the start of the file
    while (fgets(lines[nbLines], ASSEMBLY_LINE_MAX_SIZE * sizeof(char), f)) {
        // Replace the first occurence of '\n' by '\0'
        char *p = strchr(lines[nbLines], '\n');
        if (p) *p = '\0';
        // Only count not empty lines
        if (!(isBlankString(lines[nbLines]))) nbLines++;
    }

    // Convert lines into assemblyLine
    // Array of <nbLines + 1> *assemblyLine (last one being the NULL value)
    assemblyLine **assemblyLines = malloc((nbLines+1) * sizeof(assemblyLine));
    for (int i = 0; i < nbLines; i++) {
        assemblyLines[i] = readAssemblyLine(i, lines);
        // Check if the line was read correctly (if not return value is NULL)
        if (!(assemblyLines[i])) {
            printf("ERROR: INVALID ASSEMBLY LINE FORMAT\n");
            printf("       Unable to read line %d properly\n", i+1);
            // Free memory
            for (int j = i-1; j >= 0; j--) {
                free(assemblyLines[j]->label);
                free(assemblyLines[j]->instruction);
                free(assemblyLines[j]);
            }
            free(assemblyLines);
            return NULL;
        }
    }
    assemblyLines[nbLines] = NULL;

    return assemblyLines;
}


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
        printf("ERROR: INVALID ASSEMBLY FILE SYNTAX\n");
        printf("       Unable to read the assembly file content properly\n");
        return 0;
    }

    // Create the output file (in writing mode)
    FILE *outputFile = fopen(outputPath, "w");
    // If the input file was not created, end the process
    if (!(outputFile)) {
        printf("ERROR: UNABLE TO CREATE FILE\n");
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
        fprintf(outputFile, "Line data:        %d\n", assemblyLines[i]->data);
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
