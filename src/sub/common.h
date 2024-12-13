/*
    This this the header file for "common.c".

    Common elements used by most files in this project.
*/




#ifndef COMMON_H
#define COMMON_H




#include <stdio.h>
#include <stdlib.h>
#include <string.h>




#define MEMORY_REGISTRY_SIZE 5000
#define COMPILED_LINE_SIZE 8




// Return if the character is a whitespace ('\0', ' ', '\n' or '\t')
int isWhitespace(char c);


// Read and return the first "word" found, set the <str> pointer value to point after the word just read
// Words may be separated by ' ', '\n' or '\t'
// Return the NULL value if no word is found
char *readWord(char **str);


// Line of assembly code
typedef struct {
    size_t     ID;           // ID of the line, refers to the line on which it was writen in the assembly file
    size_t     number;       // Line number of the instruction, may be modified
    char      *label;        // Label of the line, may be NULL
    char      *instruction;  // Instruction of the line, may be NULL
    char      *parameter;    // Parameter of the instruction, may be NULL
} assemblyLine;




#endif // COMMON_H
