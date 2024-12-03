/*
    This this the header file for "assembly.c".

    In this file are all elements used to convert
    "assembly language" into "machine language".
*/




#ifndef ASSEMBLY_H_
#define ASSEMBLY_H_




#include <stdio.h>
#define ASSEMBLY_LINE_MAX_SIZE 32




// Convert a string into a short
// String characters must all be between '0' and '9'
// except for the first one that can be eather '-' or '+'
short stringToShort(char *s);


// Return if the string is only made of whitespaces
int isBlankString(char *s);


// Line of assembly code
typedef struct assemblyLine {
    short  number;       // Line number of the instruction
    char  *label;        // Optionnal label for the line
    char  *instruction;  // Instruction
    short  data;         // Data given to the instruction
} assemblyLine;


// Read a line of assembly code
assemblyLine *readAssemblyLine(short number, char *(lines[ASSEMBLY_LINE_MAX_SIZE]));


// Read an "assembly language" file and return a list of all the lines
assemblyLine **readAssemblyFile(FILE *f);


// Process memory
typedef struct memoryRegistry
{
    short *registry;
    short  sp;
} memoryRegistry;


// Create a new memory space
memoryRegistry *newMemoryRegistry();


// Assemble "assembly language" into "machine language"
// Return 1 if successful, 0 otherwise
int assemble(char *inputPath, char *outputPath);


#endif // ASSEMBLY_H_
