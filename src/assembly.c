/*
    In this file are all elements used to convert
    "assembly language" into "machine language".
*/




#include "assembly.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>




short stringToShort(char *str) {
    // Copy of the original string (used for debugging purposes)
    char *strCopy = str;
    // If the string is empty, return the value 0
    if (!(str)) {
        printf("WARNING: INVALID NUMBER");
        printf("         Empty string cannot be interpreted as a number");
        return 0;
    }
    // If this is a signed number
    int sign = (str[0] == '-') ? -1 : +1;
    if (str[0] == '+' || str[0] == '-') str++;
    // Read the number
    short value = 0;
    char c;
    while ((c = (str++)[0])) {
        // Each character in the string must be a number
        if (c < '0' || '9' < c) {
            printf("WARNING: INVALID NUMBER");
            printf("         String \"%s\" cannot be interpreted as a number", strCopy);
            return 0;
        }
        // If c is a valid character
        value = (10*value) + (c-'0');
    }
    return sign * value;
}


int isBlankString(char *s) {
    while (s) if (!(isspace((s++)[0]))) return 0;
    return 1;
}


assemblyLine *readAssemblyLine(short number, char *(lines[ASSEMBLY_LINE_MAX_SIZE])) {
    assemblyLine *l = malloc(sizeof(assemblyLine));
    l->number       = number;
    l->label        = NULL;
    l->instruction  = NULL;
    l->data         = 0;

    // Reading the assembly line
    // To do so, it is splited into a array of string, ignores whitespaces
    char *(tmpArray[3]) = {NULL, NULL, NULL};
    int i = 0, j = 0, k = -1;
    // While the line has not been fully read (including the '\0' character)
    while (j == 0 || lines[number][j-1]) {
        // When a whitespace (here, only '\0' and ' ') character is found
        if (lines[number][j] || lines[number][j] == ' ') {
            // If the word isn't empty
            if (j != k) {
                // tmpArray cannot contain more than 3 strings, otherwise abort process
                if (i >= 3) {
                    printf("WARNING: INVALID ASSEMBLY LINE FORMAT\n");
                    printf("         Too many elements on line %d\n", number+1);
                    for (int v = 0; v < 3; v++) free(tmpArray[i]);
                    return NULL;
                }
                // If there is space remaining inside tmpArray
                tmpArray[i] = malloc((j-k+1) * sizeof(char));
                for (int v = 0; v < j-k; v++) tmpArray[i][v] = lines[number][v+k+1];
                tmpArray[i][j-k+1] = '\0';
                i++;
            }
            k = j;
        }
        j++;
    }

    // At least 1 word has to be read
    if (i == 0) {
        printf("WARNING: INVALID ASSEMBLY LINE FORMAT\n");
        printf("         Line %d is empty\n", number+1);
        return NULL;
    }
    // 1 word was read (only an instruction)
    else if (i == 1) {
        l->label = tmpArray[0];
    }
    // 2 words were read (label, instruction OR instruction, data)
    else if (i == 2) {
        // label, instruction
        if (tmpArray[0][strlen(tmpArray[0]) - 1] == ':') {
            l->label        = tmpArray[0];
            l->instruction  = tmpArray[1];
        }
        // instruction, data
        else {
            l->instruction  = tmpArray[0];
            l->data         = stringToShort(tmpArray[1]);
        }
    }
    // 3 words were read (label, instruction, data)
    else if (i == 3) {
        l->label        = tmpArray[0];
        l->instruction  = tmpArray[1];
        l->data         = stringToShort(tmpArray[2]);
    }
    return l;
}


assemblyLine **readAssemblyFile(FILE *f) {
    // If no input file is given, abort the process
    if (!(f)) {
        printf("WARNING: MISSING FILE");
        printf("         No input file was given");
        return NULL;
    }
    // If an input file is given, continue
    char *(lines[ASSEMBLY_LINE_MAX_SIZE]);
    int nbLines = 0;
    rewind(f);  // Set cursor at the start of the file
    while (fgets(lines[nbLines], ASSEMBLY_LINE_MAX_SIZE * sizeof(char), f)) {
        // Replace the first occurence of '\n' by '\0'
        char *p = strchr(lines[nbLines], '\n');
        if (p) *p = '\0';
        // Only count not empty lines
        if (!(isBlankString(lines[nbLines]))) nbLines++;
    }
    assemblyLine *assemblyLines[nbLines];
    for (int i = 0; i < nbLines; i++) assemblyLines[i] = readAssemblyLine(i, lines);
    return assemblyLines;  // Need to use malloc here // TODO!
}


memoryRegistry *newMemoryRegistry() {
    memoryRegistry *m   = malloc(sizeof(memoryRegistry));
    m->registry         = malloc(sizeof(short) * 5000);
    m->sp               = 0;
    return m;
}


int assemble(char *inputPath, char *outputPath) {
    // Load the input file (in reading mode)
    FILE *inputFile = fopen(inputPath, "r");
    // If the input file was not found, end the process
    if (!(inputFile)) {
        printf("ERROR:   FILE NOT FOUND\n");
        printf("         Unable to find file \"%s\"\n", inputPath);
        return 0;
    }
    // If the input file was found, continue
    // Create the output file (in writing mode)
    FILE *outputFile = fopen(outputPath, "w");
    return 1;
}
