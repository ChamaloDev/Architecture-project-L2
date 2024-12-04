/*
    This this the header file for "assembly.c".

    In this file are all elements used to convert
    "assembly language" into "machine language".
*/




#ifndef ASSEMBLY_H_
#define ASSEMBLY_H_




#include <stdio.h>
#define ASSEMBLY_LINE_MAX_SIZE 32
#define MAX_ASSEMBLY_LINES 1024




// Return if the string represent a number
// All string characters must all be between '0' and '9'
// except for the first one that can also be eather '-' or '+'
int isStringNumber(char *str);


// Convert a string into a short
// The string is supposed to be representing an actual number
short stringToShort(char *str);


// Return if the string is only made of whitespaces
int isBlankString(char *str);


// Line of assembly code
typedef struct assemblyLine {
    short  number;       // Line number of the instruction
    char  *label;        // Optionnal label for the line
    char  *instruction;  // Instruction
    short  data;         // Data given to the instruction
} assemblyLine;


// Read a line of assembly code
assemblyLine *readAssemblyLine(short number, char lines[MAX_ASSEMBLY_LINES][ASSEMBLY_LINE_MAX_SIZE]);


// Read an "assembly language" file and return a list of all the lines
assemblyLine **readAssemblyFile(FILE *f);


// Assemble "assembly language" into "machine language"
// Return 1 if successful, 0 otherwise
int assemble(char *inputPath, char *outputPath);


#endif // ASSEMBLY_H_
