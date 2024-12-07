/*
    This this the header file for "read_assembly_file.c".

    In this file are all elements used to compile "assembly language"
    into "machine language".
*/




#ifndef READ_ASSEMBLY_FILE_H
#define READ_ASSEMBLY_FILE_H




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"




// Return if the string is only made of whitespaces
int isBlankString(char *str);


// A label only contains 'A-Z', 'a-z', '0-9' and '_'
// First character should be in 'A-Z' or 'a-z'
int isValidLabelName(char *str);


// Read a line of assembly code
assemblyLine *readAssemblyLine(long long number, char *line, long long lineID);


// Read an "assembly language" file and return a list of all the lines
assemblyLine **readAssemblyFile(FILE *inputFile);




#endif // READ_ASSEMBLY_FILE_H
