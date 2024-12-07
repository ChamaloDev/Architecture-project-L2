/*
    Common elements used by most files.
*/




#ifndef COMMON_H
#define COMMON_H




#define REGISTRY_SIZE 5000




// Line of assembly code
typedef struct assemblyLine {
    long long ID;            // ID of the line, refers to the line on which it was writen in the assembly file
    long long  number;       // Line number of the instruction, may be modified
    char      *label;        // Label of the line, may be NULL
    char      *instruction;  // Instruction of the line, may be NULL
    char      *parameter;    // Parameter of the instruction, may be NULL
} assemblyLine;




#endif // COMMON_H