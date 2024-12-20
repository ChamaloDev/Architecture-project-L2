/*
    This this the header file for "runner.c".

    In this file are all elements used to simulate
    the execution of "machine language".
*/




#ifndef RUNNER_H
#define RUNNER_H




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sub/common.h"




// Convert an hexadecimal string into a short
short hexaToShort(char *hexa);


// Process memory
typedef struct {
    short  *registry;  // Registries (0 - <size-1>)
    int  size;      // Maximum number of registries
    int  sp;        // Top of the temporary value pile
} memoryRegistry;


// Create a new memory space
memoryRegistry *newMemoryRegistry(int size);


// Process instruction
typedef struct {
    char  instruction;  // Instruction code of the line
    short parameter;    // Parameter of the instruction
} instructionLine;


// Read an instruction writen in "machine language"
instructionLine *readCompiledLine(char *line);


// Process list of instruction
typedef struct {
    instructionLine **instructions;   // Instruction of the process
    int               nbInstruction;  // Number of instruction
    int               pc;             // Index of the next instruction to run
} programData;


// Read a "machine language" file content
programData *readCompiledFile(char *filePath);


// Process
typedef struct {
    memoryRegistry *memory;   // Memory of the process
    programData    *program;  // Program runned by the process
} process;


// Create a new process
process *newProcess(char *filePath, int memorySize);


// Get a pointer to the registry <number>
// If the registry do not exist, return the NULL value
short *getRegistry(process *process, int number);


// Set the registry <number> to the value <value>
// Return 1 if successful in doing so, 0 otherwise
int setRegistry(process *process, int number, short value);


// Unstack and return the registry on top of the stack
short *popRegistry(process *process);


// Stack a value on top of the registry stack
int pushRegistry(process *process, short value);


// Run a process
// Return 1 if successful, 0 otherwise
int runProcess(process *process);


// Kill a process
// Free any memory used by the process
void killProcess(process *process);


// Load, run and kill a process
// Return 1 if successful in doing so, 0 otherwise
int executeProcess(char *filePath, int memory_size);




#endif // RUNNER_H
