/*
    In this file are all elements used to read the content
    of an "assembly language" file.
*/




#include "read_assembly_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>




int isValidNumber(char *str) {
    // Missing string are not numbers
    if (!(str)) return 0;
    // If this is a signed number, remove the sign
    if (str[0] == '+' || str[0] == '-') str++;
    int i = 0;
    for (; '0' <= str[i] && str[i] <= '9'; i++);
    // There must be at least 1 number, and the last character reached should be '\0'
    return (i && !(str[i]));
}


short stringToShort(char *str) {
    if (!(str)) return 0;
    // If this is a signed number
    int sign = (str[0] == '-') ? -1 : +1;
    if (str[0] == '+' || str[0] == '-') str++;
    // Read the number
    short value = 0;
    char c;
    while ((c = (str++)[0])) value = (10*value) + (c-'0');
    return sign * value;
}


int isBlankString(char *str) {
    if (!(str)) return 1;
    while (str[0]) if (!(isspace((str++)[0]))) return 0;
    return 1;
}


int isValidLabelName(char *str) {
    if (!(str)) return 0;
    // TODO!
    return 1;
}


assemblyLine *readAssemblyLine(short number, char lines[MAX_ASSEMBLY_LINES][ASSEMBLY_LINE_MAX_SIZE+1]) {
    assemblyLine *l = malloc(sizeof(assemblyLine));
    l->number       = number+1;
    l->label        = NULL;
    l->instruction  = NULL;
    l->data         = NULL;

    // Reading the assembly line
    // To do so, it is splited into a array of string, ignores whitespaces
    char *(tmpArray[3]) = {NULL, NULL, NULL};
    int i = 0, j = 0, k = 0;
    // While the line has not been fully read (including the '\0' character)
    while (j == 0 || lines[number][j-1]) {
        // When a whitespace (here, only '\0' and ' ') character is found
        if (!(lines[number][j]) || isspace(lines[number][j])) {
            // If the word isn't empty
            if (j != k) {
                // tmpArray cannot contain more than 3 strings, otherwise abort process
                if (i >= 3) {
                    printf("ERROR: ASSEMBLY SYNTAX ERROR\n");
                    printf("       Line contains too many elements\n");
                    for (int v = 0; v < 3; v++) free(tmpArray[v]);
                    return NULL;
                }
                // If there is space remaining inside tmpArray
                tmpArray[i] = malloc((j-k+1) * sizeof(char));
                for (int v = 0; v < j-k; v++) tmpArray[i][v] = lines[number][v+k];
                tmpArray[i][j-k] = '\0';
                i++;
            }
            k = j+1;
        }
        j++;
    }

    // At least 1 word has to be read
    if (i == 0) {
        printf("ERROR: ASSEMBLY SYNTAX ERROR\n");
        printf("       Empty line\n");
        return NULL;
    }
    // 1 word was read (label OR instruction)
    else if (i == 1) {
        // label
        if (tmpArray[0][strlen(tmpArray[0]) - 1] == ':') {
            // Replace the ':' character by '\0' in the label name
            tmpArray[0][strlen(tmpArray[0]) - 1] = '\0';
            if (!(isValidLabelName(tmpArray[0]))) {
                printf("ERROR: INVALID LABEL NAME\n");
                printf("       String \"%s\" isn't allowed as a label name\n", tmpArray[0]);
                free(tmpArray[0]);
                return NULL;
            }
            l->label = tmpArray[0];
        }
        // instruction
        else l->instruction = tmpArray[0];
    }
    // 2 words were read (label, instruction OR instruction, data)
    else if (i == 2) {
        // label, instruction
        if (tmpArray[0][strlen(tmpArray[0]) - 1] == ':') {
            tmpArray[0][strlen(tmpArray[0]) - 1] = '\0';
            // Check the validity of the label name
            if (!(isValidLabelName(tmpArray[0]))) {
                printf("ERROR: INVALID LABEL NAME\n");
                printf("       String \"%s\" isn't allowed as a label name\n", tmpArray[0]);
                for (int v = 0; v < 2; v++) free(tmpArray[v]);
                return NULL;
            }
            l->label        = tmpArray[0];
            l->instruction  = tmpArray[1];
        }
        // instruction, data
        else {
            l->instruction  = tmpArray[0];
            l->data         = tmpArray[1];
        }
    }
    // 3 words were read (label, instruction, data)
    else if (i == 3) {
        // Check if the string representing the label ends with ':', if not abort process
        if (tmpArray[0][strlen(tmpArray[0]) - 1] != ':') {
            printf("ERROR: INVALID LABEL NAME\n");
            printf("       String \"%s\" must end with ':' to be read as a label\n", tmpArray[0]);
            for (int v = 0; v < 3; v++) free(tmpArray[v]);
            return NULL;
        }
        // Replace the ':' character by '\0' in the label name
        tmpArray[0][strlen(tmpArray[0]) - 1] = '\0';
        // Check the validity of the label name
        if (!(isValidLabelName(tmpArray[0]))) {
            printf("ERROR: INVALID LABEL NAME\n");
            printf("       String \"%s\" isn't allowed as a label name\n", tmpArray[0]);
            for (int v = 0; v < 3; v++) free(tmpArray[v]);
            return NULL;
        }
        l->label        = tmpArray[0];
        l->instruction  = tmpArray[1];
        l->data         = tmpArray[2];
    }

    return l;
}


assemblyLine **readAssemblyFile(FILE *inputFile) {
    // If no input file is given, abort the process
    if (!(inputFile)) {
        printf("ERROR: MISSING FILE\n");
        printf("       No input file was given\n");
        return NULL;
    }

    // Read file content
    char lines[MAX_ASSEMBLY_LINES][ASSEMBLY_LINE_MAX_SIZE+1];
    char buffer[ASSEMBLY_LINE_MAX_SIZE+2];  // Buffer used for the assembly line
    int nbLines = 0;
    rewind(inputFile);  // Set cursor at the start of the file
    while (fgets(buffer, (ASSEMBLY_LINE_MAX_SIZE+2) * sizeof(char), inputFile)) {
        // Replace the first occurence of '\n' by '\0'
        char *p = strchr(buffer, '\n');
        if (p) *p = '\0';
        // Check if the line is too long or not
        if (strnlen(buffer, ASSEMBLY_LINE_MAX_SIZE+2) > ASSEMBLY_LINE_MAX_SIZE) {
            printf("ERROR: MEMORY ERROR\n");
            printf("       Limit of %d characters per line in assembly line %d has been reached\n", ASSEMBLY_LINE_MAX_SIZE, nbLines+1);
            return NULL;
        }
        // Only count not empty lines
        if (!(isBlankString(buffer))) {
            // Error if max line amount is reached
            if (nbLines >= MAX_ASSEMBLY_LINES) {
                printf("ERROR: MEMORY ERROR\n");
                printf("       Limit of %d assembly lines has been reached\n", MAX_ASSEMBLY_LINES);
                return NULL;
            }
            strncpy(lines[nbLines], buffer, ASSEMBLY_LINE_MAX_SIZE+1);  // Paste the content of the buffer into its desired location
            nbLines++;
        }
    }

    // Convert lines into assemblyLine
    // Array of <nbLines + 1> *assemblyLine (last one being the NULL value)
    assemblyLine **assemblyLines = malloc((nbLines+1) * sizeof(assemblyLine));
    for (int i = 0; i < nbLines; i++) {
        assemblyLines[i] = readAssemblyLine(i, lines);
        // Check if the line was read correctly (if not return value is NULL)
        if (!(assemblyLines[i])) {
            printf("ERROR: ASSEMBLY SYNTAX ERROR\n");
            printf("       Unable to read line %d properly\n", i+1);
            // Free memory
            for (int j = i-1; j >= 0; j--) {
                free(assemblyLines[j]->label);
                free(assemblyLines[j]->instruction);
                free(assemblyLines[j]->data);
                free(assemblyLines[j]);
            }
            free(assemblyLines);
            return NULL;
        }
    }
    assemblyLines[nbLines] = NULL;

    return assemblyLines;
}
