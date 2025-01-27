/*
    In this file are all elements used to read the content
    of an "assembly language" file.
*/




#include "read_assembly_file.h"




int isBlankString(char *str) {
    if (!(str)) return 1;
    while (str[0]) if (!(isWhitespace((str++)[0]))) return 0;
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


assemblyLine *readAssemblyLine(int number, char *line, int lineID) {
    // Initializing 
    assemblyLine *l = malloc(sizeof(assemblyLine));
    l->ID          = lineID;
    l->number      = number+1;
    l->label       = NULL;
    l->instruction = NULL;
    l->parameter   = NULL;

    // Reading the assembly line by spliting it into an array of strings, split on whitespaces
    char *(tmpArray[4]) = {NULL, NULL, NULL, NULL};
    int nbElem = 0;
    while (nbElem < 4) {
        tmpArray[nbElem] = readWord(&line);
        if (!(tmpArray[nbElem])) break;
        nbElem ++;
    }

    // 0 word was read (throw an error)
    if (nbElem == 0) {
        printf("ERROR: ASSEMBLY SYNTAX ERROR\n");
        printf("       Line %d is empty\n", l->ID);
        return NULL;
    }
    // 1 word was read (label OR instruction)
    else if (nbElem == 1) {
        // label
        if (tmpArray[0][strlen(tmpArray[0]) - 1] == ':') {
            // Replace the ':' character by '\0' in the label name
            tmpArray[0][strlen(tmpArray[0]) - 1] = '\0';
            if (!(isValidLabelName(tmpArray[0]))) {
                printf("ERROR: INVALID LABEL NAME\n");
                printf("       On line %d, name \"%s\" cannot be used as label\n", l->ID, tmpArray[0]);
                free(tmpArray[0]);
                return NULL;
            }
            l->label = tmpArray[0];
        }
        // instruction
        else l->instruction = tmpArray[0];
    }
    // 2 words were read (label, instruction OR instruction, parameter)
    else if (nbElem == 2) {
        // label, instruction
        if (tmpArray[0][strlen(tmpArray[0]) - 1] == ':') {
            tmpArray[0][strlen(tmpArray[0]) - 1] = '\0';
            // Check the validity of the label name
            if (!(isValidLabelName(tmpArray[0]))) {
                printf("ERROR: INVALID LABEL NAME\n");
                printf("       On line %d, name \"%s\" cannot be used as label\n", l->ID, tmpArray[0]);
                for (int i = 0; i < 2; i++) free(tmpArray[i]);
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
    else if (nbElem == 3) {
        // Check if the string representing the label ends with ':', if not throw error
        if (tmpArray[0][strlen(tmpArray[0]) - 1] != ':') {
            printf("ERROR: INVALID LABEL NAME\n");
            printf("       On line %d, name \"%s\" must end with ':'\n", l->ID, tmpArray[0]);
            for (int i = 0; i < 3; i++) free(tmpArray[i]);
            return NULL;
        }
        // Replace the ':' character by '\0' in the label name
        tmpArray[0][strlen(tmpArray[0]) - 1] = '\0';
        // Check the validity of the label name
        if (!(isValidLabelName(tmpArray[0]))) {
            printf("ERROR: INVALID LABEL NAME\n");
            printf("       On line %d, name \"%s\" cannot be used as label\n", l->ID, tmpArray[0]);
            for (int i = 0; i < 3; i++) free(tmpArray[i]);
            return NULL;
        }
        l->label       = tmpArray[0];
        l->instruction = tmpArray[1];
        l->parameter   = tmpArray[2];
    }
    // 4+ words were read (throw an error)
    else {
        printf("ERROR: ASSEMBLY SYNTAX ERROR\n");
        printf("       Line %d contains too many elements\n", l->ID);
        for (int i = 0; i < 4; i++) free(tmpArray[i]);
        return NULL;
    }

    return l;
}


assemblyLine **readAssemblyFile(FILE *inputFile) {
    int         max_line_count = 32;
    int         line_max_size  = 32;
    // Array of *assemblyLine (last one being the NULL value)
    assemblyLine **assemblyCode   = malloc((max_line_count+1) * sizeof(assemblyLine *));
    char          *line           = malloc((line_max_size+1)  * sizeof(char));
    int         lineNb         = 0;
    int         lineID         = 1;
    // Set cursor at the start of the file
    rewind(inputFile);
    // Read file content and convert it into assembly lines
    while (fgets(line, line_max_size+1, inputFile)) {
    	char *p = NULL;
        // If more memory is needed
        if (strlen(line) >= (size_t) line_max_size) {
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
        // Ignore comments (text after the ';' character)
        // This step MUST be done AFTER memory reallocation check
    	p = strchr(line, ';');
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
