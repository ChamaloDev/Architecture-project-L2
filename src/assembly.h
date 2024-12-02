/*
    This this the header file for "assembly.c".

    In this file are all elements used to convert
    "assembly language" into "machine language".
*/


#ifndef ASSEMBLY_H_
#define ASSEMBLY_H_


// Line of assembly code
typedef struct assemblyLine {
    short  number;
    char  *label;
    char  *operation;
    short  value;
} assemblyLine;

// Read a line of assembly code
assemblyLine *readAssemblyLine(short number, char *line);

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
