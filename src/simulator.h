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
} instruction;


// Process list of instruction
typedef struct {
    instruction **instructions;   // Instruction of the process
    long long     nbInstruction;  // Number of instruction
    long long     pc;             // Index of the net instruction to run
} instructionList;


// Process
typedef struct {
    memoryRegistry  *memory;   // Memory of the process
    instructionList *program;  // Program runned by the process
} process;


// TODO!




#endif // SIMULATOR_H
