/*
    In this file are all elements used to compile "assembly language"
    into "machine language".
*/




#include "compile_assembly_code.h"




long long max(long long a, long long b) {
    return ((a > b) ? (a) : (b));
}


void appendToString(char *str, char *value) {
    strcpy(str + strlen(str), value);
}


char *hexadecimal(short value, size_t size) {
    // Temporary hexadecimal string (likely to have wrong size)
    char tmp[9];
    sprintf(tmp, "%x", value);
    // Definitive hexadecimal string, initialised to '0' * <size> + '\0'
    char *hexa = malloc((size+1) * sizeof(char));
    for (size_t i = 0; i < size; i++) hexa[i] = '0';
    hexa[size] = '\0';
    // Copying <tmp> into <hexa> (restricting it to <size> characters)
    size_t sizeTmp = strlen(tmp);
    strcpy(hexa + max(size - sizeTmp, 0), tmp + max(sizeTmp - size, 0));
    return hexa;
}


char *getCompiledFormOf(char instructionID, short parameterValue) {
    char *compiledLine = malloc((COMPILED_LINE_SIZE+1) * sizeof(char));
    char *hexa;
    compiledLine[0] = '\0';
    // Hexadecimal code of instruction
    hexa = hexadecimal(instructionID, 2);
    appendToString(compiledLine, hexa);
    free(hexa);
    // Separation space
    appendToString(compiledLine, " ");
    // Hexadecimal value of parameter
    hexa = hexadecimal(parameterValue, 4);
    appendToString(compiledLine, hexa);
    free(hexa);
    // End of line
    appendToString(compiledLine, "\n");
    return compiledLine;
}


int isValidNumber(char *str) {
    // Missing string are not numbers
    if (!(str)) return 0;
    // If this is a signed number, remove the sign
    if (str[0] == '+' || str[0] == '-') str++;
    size_t i = 0;
    while ('0' <= str[i] && str[i] <= '9') i++;
    // There must be at least 1 number, and the last character reached should be '\0'
    return (i && !(str[i]));
}


short stringToShort(char *str) {
    if (!(str)) return 0;
    // If this is a signed number
    int sign = ((str[0] == '-') ? (-1) : (+1));
    if (str[0] == '+' || str[0] == '-') str++;
    // Read the number
    short value = 0;
    char c;
    while ((c = (str++)[0])) value = (10*value) + (c-'0');
    return sign * value;
}


char *shortToString(short n) {
    char *str = malloc(7 * sizeof(char));
    // Number sign
    str[0] = ((n < 0) ? ('-') : ('+'));
    // Number absolute value
    str[1] = abs(n / 10000) % 10 + '0';
    str[2] = abs(n / 1000 ) % 10 + '0';
    str[3] = abs(n / 100  ) % 10 + '0';
    str[4] = abs(n / 10   ) % 10 + '0';
    str[5] = abs(n / 1    ) % 10 + '0';
    // End of string
    str[6] = '\0';
    return str;
}


int hasValidRegistry(assemblyLine *line) {
    // Instruction needs a parameter
    if (!(line->parameter)) {
        printf("ERROR: MISSING PARAMETER\n");
        printf("       On line %lld, intruction \"%s\" needs a registry (0 - %d) as parameter\n", line->ID, line->instruction, MEMORY_REGISTRY_SIZE-1);
        return 0;
    }
    // Instruction needs a number as parameter
    if (!(isValidNumber(line->parameter))) {
        printf("ERROR: INVALID PARAMETER\n");
        printf("       On line %lld, intruction \"%s\" needs a registry (0 - %d) as parameter, got \"%s\" instead\n", line->ID, line->instruction, MEMORY_REGISTRY_SIZE-1, line->parameter);
        return 0;
    }
    // Instruction needs a number between 0 and <MEMORY_REGISTRY_SIZE-1> as parameter
    int value = stringToShort(line->parameter);
    if (0 > value || value > MEMORY_REGISTRY_SIZE-1) {
        printf("ERROR: INVALID PARAMETER\n");
        printf("       On line %lld, intruction \"%s\" needs a registry (0 - %d) as parameter, got %d instead\n", line->ID, line->instruction, MEMORY_REGISTRY_SIZE-1, value);
        return 0;
    }
    return 1;
}


int hasValidConstant(assemblyLine *line) {
    // Instruction needs a parameter
    if (!(line->parameter)) {
        printf("ERROR: MISSING PARAMETER\n");
        printf("       On line %lld, intruction \"%s\" needs a constant value (-32768 - +32767) as parameter\n", line->ID, line->instruction);
        return 0;
    }
    // Instruction needs a number as parameter
    if (!(isValidNumber(line->parameter))) {
        printf("ERROR: INVALID PARAMETER\n");
        printf("       On line %lld, intruction \"%s\" needs a constant value (-32768 - +32767) as parameter, got \"%s\" instead\n", line->ID, line->instruction, line->parameter);
        return 0;
    }
    return 1;
}


int hasValidLabel(assemblyLine *line) {
    // Instruction needs a parameter
    if (!(line->parameter)) {
        printf("ERROR: MISSING PARAMETER\n");
        printf("       On line %lld, intruction \"%s\" needs a label as parameter\n", line->ID, line->instruction);
        return 0;
    }
    // Instruction needs a number as parameter
    if (!(isValidNumber(line->parameter))) {
        printf("ERROR: LABEL NOT FOUND\n");
        printf("       On line %lld, label \"%s\" has not been defined\n", line->ID, line->parameter);
        return 0;
    }
    return 1;
}


int hasNoParameter(assemblyLine *line) {
    // Instruction cannot have a parameter
    if (line->parameter) {
        printf("ERROR: INVALID PARAMETER\n");
        printf("       On line %lld, intruction \"%s\" cannot take a parameter\n", line->ID, line->instruction);
        return 0;
    }
    return 1;
}


int compileLine(char *compiledCode, assemblyLine *line) {
    // Only a label (nothing to do)
    if (!(line->instruction)) return 1;
    // Instruction (00) pop x
    if (!(strcmp(line->instruction, "pop"))) {
        if (!(hasValidRegistry(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(0, stringToShort(line->parameter)));
        return 1;
    }
    // Instruction (01) ipop
    if (!(strcmp(line->instruction, "ipop"))) {
        if (!(hasNoParameter(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(1, 0));
        return 1;
    }
    // Instruction (02) push x
    if (!(strcmp(line->instruction, "push"))) {
        if (!(hasValidRegistry(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(2, stringToShort(line->parameter)));
        return 1;
    }
    // Instruction (03) ipush
    if (!(strcmp(line->instruction, "ipush"))) {
        if (!(hasNoParameter(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(3, 0));
        return 1;
    }
    // Instruction (04) push# i
    if (!(strcmp(line->instruction, "push#"))) {
        if (!(hasValidConstant(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(4, stringToShort(line->parameter)));
        return 1;
    }
    // Instruction (05) jmp adr
    if (!(strcmp(line->instruction, "jmp"))) {
        if (!(hasValidLabel(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(5, stringToShort(line->parameter)));
        return 1;
    }
    // Instruction (06) jnz adr
    if (!(strcmp(line->instruction, "jnz"))) {
        if (!(hasValidLabel(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(6, stringToShort(line->parameter)));
        return 1;
    }
    // Instruction (07) call adr
    if (!(strcmp(line->instruction, "call"))) {
        if (!(hasValidLabel(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(7, stringToShort(line->parameter)));
        return 1;
    }
    // Instruction (08) ret
    if (!(strcmp(line->instruction, "ret"))) {
        if (!(hasNoParameter(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(8, 0));
        return 1;
    }
    // Instruction (09) read x
    if (!(strcmp(line->instruction, "read"))) {
        if (!(hasValidRegistry(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(9, stringToShort(line->parameter)));
        return 1;
    }
    // Instruction (10) write x
    if (!(strcmp(line->instruction, "write"))) {
        if (!(hasValidRegistry(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(10, stringToShort(line->parameter)));
        return 1;
    }
    // Instruction (11) op i
    if (!(strcmp(line->instruction, "op"))) {
        if (!(hasValidConstant(line))) return 0;
        int value = stringToShort(line->parameter);
        if (0 > value || value > 15) {
            printf("ERROR: INVALID PARAMETER\n");
            printf("       On line %lld, intruction \"%s\" needs an operation code (0 - 15) as parameter, got %s instead\n", line->ID, line->instruction, line->parameter);
            return 0;
        }
        appendToString(compiledCode, getCompiledFormOf(11, value));
        return 1;
    }
    // Instruction (12) rnd x
    if (!(strcmp(line->instruction, "rnd"))) {
        if (!(hasValidRegistry(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(12, stringToShort(line->parameter)));
        return 1;
    }
    // Instruction (13) dup
    if (!(strcmp(line->instruction, "dup"))) {
        if (!(hasNoParameter(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(13, 0));
        return 1;
    }
    // Instruction (99) halt
    if (!(strcmp(line->instruction, "halt"))) {
        if (!(hasNoParameter(line))) return 0;
        appendToString(compiledCode, getCompiledFormOf(99, 0));
        return 1;
    }
    // Invalid instruction
    printf("ERROR: INVALID INSTRUCTION\n");
    printf("       On line %lld, instruction \"%s\" is not defined\n", line->ID, line->instruction);
    return 0;
}


char *compile(assemblyLine **assemblyCode) {
    // Count number of lines that will be writen and adjust their line number
    size_t lineNb = 0;
    for (size_t i = 0; assemblyCode[i]; i++) {
        assemblyCode[i]->number = lineNb+1;
        if (assemblyCode[i]->instruction) lineNb++;
    }

    // Look for duplicate label name and replace labels by their value
    char *tmp;
    for (size_t i = 0; assemblyCode[i]; i++) {
        if (assemblyCode[i]->label) for (size_t j = 0; assemblyCode[j]; j++) {
            // If a duplicate is found in label names, throw error
            if (j > i && assemblyCode[j]->label && !(strcmp(assemblyCode[i]->label, assemblyCode[j]->label))) {
                printf("ERROR: DUPLICATE LABEL NAME\n");
                printf("       Label \"%s\" has duplicates, on lines %lld and %lld\n", assemblyCode[i]->label, assemblyCode[i]->ID, assemblyCode[j]->ID);
                return NULL;
            }
            // Replacing label by its value
            if (assemblyCode[j]->parameter && !(strcmp(assemblyCode[i]->label, assemblyCode[j]->parameter))) {
                // Check if the instruction do take a label as parameter, if not throw an error
                if (strcmp(assemblyCode[j]->instruction, "jmp") && strcmp(assemblyCode[j]->instruction, "jnz") && strcmp(assemblyCode[j]->instruction, "call")) {
                    printf("ERROR: INVALID PARAMETER\n");
                    printf("       On line %lld, intruction \"%s\" cannot take label \"%s\" as parameter\n", assemblyCode[j]->ID, assemblyCode[j]->instruction, assemblyCode[j]->parameter);
                    return NULL;
                }
                tmp = shortToString(assemblyCode[i]->number - assemblyCode[j]->number - 1);
                free(assemblyCode[j]->parameter);
                assemblyCode[j]->parameter = tmp;
            }
        }
    }

    char *compiledCode = malloc((lineNb*COMPILED_LINE_SIZE+1) * sizeof(char));
    compiledCode[0] = '\0';
    // Compile all assembly lines
    int result;
    for (size_t i = 0; assemblyCode[i]; i++) {
        result = compileLine(compiledCode, assemblyCode[i]);
        // If unable to compile assembly line, throw error
        if (!(result)) {
            free(compiledCode);
            return NULL;
        }
    }

    return compiledCode;
}
