/*
    In this file are all elements used to read the content
    of an "assembly language" file.
*/




#include "read_assembly_file.h"




int isWhiteSpace(char c) {
    return (!(c)) || (c == ' ') || (c == '\n') || (c == '\t');
}


int isBlankString(char *str) {
    if (!(str)) return 1;
    while (str[0]) if (!(isWhiteSpace((str++)[0]))) return 0;
    return 1;
}


int isValidLabelName(char *str) {
    if (!(str)) return 0;
    // Condition on first character (should be in 'A-Z', 'a-z' or equal to '_'), meaning labels cannot be empty string
    if (!(('A' <= str[0] && str[0] <= 'Z') || ('a' <= str[0] && str[0] <= 'z') || (str[0] == '_'))) return 0;
    // Condition on all other characters (should be in 'A-Z', 'a-z', '0-9' or equal to '_')
    while ((++str)[0]) if (!(('A' <= str[0] && str[0] <= 'Z') || ('a' <= str[0] && str[0] <= 'z') || ('0' <= str[0] && str[0] <= '9') || (str[0] == '_'))) return 0;
    return 1;
}


assemblyLine *readAssemblyLine(long long number, char *line, long long lineID) {
    assemblyLine *l = malloc(sizeof(assemblyLine));
    l->ID          = lineID;
    l->number      = number+1;
    l->label       = NULL;
    l->instruction = NULL;
    l->parameter   = NULL;

    // Reading the assembly line
    // To do so, it is splited into a array of string, ignores whitespaces
    char *(tmpArray[3]) = {NULL, NULL, NULL};
    int i = 0, j = 0, k = 0;
    // While the line has not been fully read (including the '\0' character)
    while (j == 0 || line[j-1]) {
        // When a whitespace ('\0', ' ', '\n' or '\t') character is found
        if (isWhiteSpace(line[j])) {
            // If the word isn't empty
            if (j != k) {
                // tmpArray cannot contain more than 3 strings, otherwise throw error
                if (i >= 3) {
                    printf("ERROR: ASSEMBLY SYNTAX ERROR\n");
                    printf("       Line %lld contains too many elements\n", l->ID);
                    for (int v = 0; v < 3; v++) free(tmpArray[v]);
                    return NULL;
                }
                // If there is space remaining inside tmpArray
                tmpArray[i] = malloc((j-k+1) * sizeof(char));
                for (int v = 0; v < j-k; v++) tmpArray[i][v] = line[v+k];
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
        printf("       Line %lld is empty\n", l->ID);
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
                printf("       On line %lld, name \"%s\" cannot be used as label\n", l->ID, tmpArray[0]);
                free(tmpArray[0]);
                return NULL;
            }
            l->label = tmpArray[0];
        }
        // instruction
        else l->instruction = tmpArray[0];
    }
    // 2 words were read (label, instruction OR instruction, parameter)
    else if (i == 2) {
        // label, instruction
        if (tmpArray[0][strlen(tmpArray[0]) - 1] == ':') {
            tmpArray[0][strlen(tmpArray[0]) - 1] = '\0';
            // Check the validity of the label name
            if (!(isValidLabelName(tmpArray[0]))) {
                printf("ERROR: INVALID LABEL NAME\n");
                printf("       On line %lld, name \"%s\" cannot be used as label\n", l->ID, tmpArray[0]);
                for (int v = 0; v < 2; v++) free(tmpArray[v]);
                return NULL;
            }
            l->label       = tmpArray[0];
            l->instruction = tmpArray[1];
        }
        // instruction, parameter
        else {
            l->instruction = tmpArray[0];
            l->parameter   = tmpArray[1];
        }
    }
    // 3 words were read (label, instruction, parameter)
    else if (i == 3) {
        // Check if the string representing the label ends with ':', if not throw error
        if (tmpArray[0][strlen(tmpArray[0]) - 1] != ':') {
            printf("ERROR: INVALID LABEL NAME\n");
            printf("       On line %lld, name \"%s\" must end with ':'\n", l->ID, tmpArray[0]);
            for (int v = 0; v < 3; v++) free(tmpArray[v]);
            return NULL;
        }
        // Replace the ':' character by '\0' in the label name
        tmpArray[0][strlen(tmpArray[0]) - 1] = '\0';
        // Check the validity of the label name
        if (!(isValidLabelName(tmpArray[0]))) {
            printf("ERROR: INVALID LABEL NAME\n");
            printf("       On line %lld, name \"%s\" cannot be used as label\n", l->ID, tmpArray[0]);
            for (int v = 0; v < 3; v++) free(tmpArray[v]);
            return NULL;
        }
        l->label       = tmpArray[0];
        l->instruction = tmpArray[1];
        l->parameter   = tmpArray[2];
    }

    return l;
}


assemblyLine **readAssemblyFile(FILE *inputFile) {
    long long      max_line_count = 32;
    long long      line_max_size  = 32;
    // Array of *assemblyLine (last one being the NULL value)
    assemblyLine **assemblyCode   = malloc((max_line_count+1) * sizeof(assemblyLine *));
    char          *line           = malloc((line_max_size+1)  * sizeof(char));
    long long      lineNb         = 0;
    long long      lineID         = 1;
    // Set cursor at the start of the file
    rewind(inputFile);
    // Read file content and convert it into assembly lines
    while (fgets(line, line_max_size+1, inputFile)) {
    	char *p = NULL;
        // If more memory is needed
        if (strlen(line) >= line_max_size) {
            // Try reading again the same line by setting the cursor back to the start of the line
            fseek(inputFile, -(line_max_size), SEEK_CUR);
            // Extend line buffer size
            line_max_size *= 2;
            line = realloc(line, (line_max_size+1) * sizeof(char));
            // Go back to the start of the loop to try and read the line again
            continue;
        }

        // Replace the first occurence of '\n' by '\0'
        p = strchr(line, '\n');
        if (p) *p = '\0';
        // Ignore comments (text after the '/' character)
        // This step MUST be done AFTER memory reallocation check
    	p = strchr(line, '/');
        if (p) *p = '\0';

        // Only count not empty lines
        if (!(isBlankString(line))) {
            // Check if the line was read correctly (if not return value is NULL)
            assemblyCode[lineNb] = readAssemblyLine(lineNb, line, lineID);
            if (!(assemblyCode[lineNb])) {
                // Free memory
                for (int j = lineNb-1; j >= 0; j--) {
                    free(assemblyCode[j]->label);
                    free(assemblyCode[j]->instruction);
                    free(assemblyCode[j]->parameter);
                    free(assemblyCode[j]);
                }
                free(assemblyCode);
                return NULL;
            }
            // Count the line
            lineNb++;
            // If more memory is needed
            if (lineNb > max_line_count) {
                // Extend file buffer size
                max_line_count *= 2;
                assemblyCode = realloc(assemblyCode, (max_line_count+1) * sizeof(assemblyLine *));
            }
        }
        // No matter what, increase lineID by 1
        lineID++;
    }
    // Set the last element to be a NULL pointer
    assemblyCode[lineNb] = NULL;

    return assemblyCode;
}
