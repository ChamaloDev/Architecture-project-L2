/*
    This this the header file for "read_assembly_file.c".

    In this file are all elements used to compile "assembly language"
    into "machine language".
*/




#ifndef READ_ASSEMBLY_FILE_H
#define READ_ASSEMBLY_FILE_H




#include <stdio.h>




// Return if the string represent a number
// All string characters must all be between '0' and '9'
// except for the first one that can also be eather '-' or '+'
int isValidNumber(char *str);


// Convert a string into a short
// The string is supposed to be representing an actual number
short stringToShort(char *str);


// Return if the string is only made of whitespaces
int isBlankString(char *str);


// A label only contains 'A-Z', 'a-z', '0-9' and '_'
// First character should be in 'A-Z' or 'a-z'
int isValidLabelName(char *str);


// Line of assembly code
typedef struct assemblyLine {
    short  number;       // Line number of the instruction
    char  *label;        // Optionnal label for the line
    char  *instruction;  // Instruction
    char  *data;         // Data given to the instruction
} assemblyLine;


// Read a line of assembly code
assemblyLine *readAssemblyLine(short number, char *line);


// Read an "assembly language" file and return a list of all the lines
assemblyLine **readAssemblyFile(FILE *inputFile);




#endif // READ_ASSEMBLY_FILE_H
