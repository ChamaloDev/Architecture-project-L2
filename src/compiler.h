/*
    This this the header file for "compiler.c".

    In this file are all elements used to convert "assembly language"
    into "machine language".
*/




#ifndef ASSEMBLY_H
#define ASSEMBLY_H




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sub/read_assembly_file.h"
#include "sub/compile_assembly_code.h"




// Assemble "assembly language" into "machine language"
// Return 1 if successful, 0 otherwise
int assemble(char *inputPath, char *outputPath);




#endif // ASSEMBLY_H
