/*
    This this the header file for "compile_assembly_code.c".

    In this file are all elements used to compile "assembly language"
    into "machine language".
*/




#ifndef COMPILE_ASSEMBLY_CODE_H
#define COMPILE_ASSEMBLY_CODE_H




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"




// Return the maximum between <a> and <b>
int max(int a, int b);


// Append the string <value> at the end of the string <str>
void appendToString(char *str, char *value);


// Convert <number> into an hexadecimal string made of <size> characters
char *hexadecimal(int value, int size);


// Get the associated line in hexadecimal
char *getCompiledFormOf(char instructionID, short parameterValue);


// Return if the string represent a number
// All string characters must all be between '0' and '9'
// except for the first one that can also be eather '-' or '+'
int isValidNumber(char *str);


// Convert a string into a short
// The string is supposed to be representing an actual number
short stringToShort(char *str);


// Convert a short into a string
char *shortToString(short n);


// Handle error for instructions that take a registry value as parameter
// Return 1 if no error found, 0 otherwise
int hasValidRegistry(assemblyLine *line);


// Handle error for instructions that take a constant value as parameter
// Return 1 if no error found, 0 otherwise
int hasValidConstant(assemblyLine *line);


// Handle error for instructions that take a label as parameter
// Return 1 if no error found, 0 otherwise
int hasValidLabel(assemblyLine *line);


// Handle error for instructions that take no parameter
// Return 1 if no error found, 0 otherwise
int hasNoParameter(assemblyLine *line);


// Compile a singular assembly line into "machine language"
int compileLine(char *compiledCode, assemblyLine *line);


// Compile "assembly language" into "machine language"
char *compile(assemblyLine **assemblyCode);




#endif // COMPILE_ASSEMBLY_CODE_H
