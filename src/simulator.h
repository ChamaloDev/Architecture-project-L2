/*
    This this the header file for "simulator.c".

    In this file are all elements used to simulate
    the execution of "machine language".
*/




#ifndef SIMULATOR_H
#define SIMULATOR_H




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sub/common.h"




// Convert an hexadecimal string into a short
short hexaToShort(char *hexa);


// Process memory
typedef struct {
    short     *registry;  // Registries (0 - <size-1>)
    long long  size;      // Maximum number of registries
    long long  sp;        // Top of the temporary value pile
} memoryRegistry;


// Create a new memory space
memoryRegistry *newMemoryRegistry(long long size);


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
    long long         nbInstruction;  // Number of instruction
    long long         pc;             // Index of the next instruction to run
} instructionList;


// Read a "machine language" file content
instructionList *readCompiledFile(char *filePath);


// Process
typedef struct {
    memoryRegistry  *memory;   // Memory of the process
    instructionList *program;  // Program runned by the process
} process;


// Create a new process
process *newProcess(char *filePath, long long memorySize);


// Run a process
void runProcess(process *p);


// TODO!




#endif // SIMULATOR_H
