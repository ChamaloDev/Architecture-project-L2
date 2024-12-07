/*
    This this the header file for "simulator.c".

    In this file are all elements used to simulate
    the execution of "machine language".
*/


#ifndef SIMULATOR_H
#define SIMULATOR_H




// Process memory
typedef struct memoryRegistry
{
    short *registry;  // Registries
    short  sp;        // Top of the pile
} memoryRegistry;


// Create a new memory space
memoryRegistry *newMemoryRegistry();


// TODO!




#endif // SIMULATOR_H
