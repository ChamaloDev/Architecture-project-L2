/*
    In this file are all elements used to simulate
    the execution of "machine language".
*/




#include "runner.h"




short hexaToShort(char *hexa) {
    if (!(hexa)) return 0;
    short value = 0;
    for (int i = 0; hexa[i]; i++) value = 16*value + (('0' <= hexa[i] && hexa[i] <= '9') ? (hexa[i] - '0') : ((hexa[i] - 'a') + 10));
    return value;
}


memoryRegistry *newMemoryRegistry(int size) {
    memoryRegistry *memo = malloc(sizeof(memoryRegistry));
    memo->registry       = malloc(size * sizeof(short));
    memo->size           = size;
    memo->sp             = 0;
    return memo;
}


instructionLine *readCompiledLine(char *line) {
    // Spliting the line into its 2 elements : instruction code and parameter
    char            *instr     = readWord(&line);
    char            *para      = readWord(&line);
    // Initializing the instructionLine object
    instructionLine *instrLine = malloc(sizeof(instructionLine));
    instrLine->instruction     = hexaToShort(instr);
    instrLine->parameter       = hexaToShort(para);
    // Free memory
    free(instr);
    free(para);
    return instrLine;
}


programData *readCompiledFile(char *filePath) {
    // Load the file (in reading mode)
    FILE *file = fopen(filePath, "r");
    // If the file was not found, throw an error
    if (!(file)) {
        printf("ERROR: FILE NOT FOUND\n");
        printf("       Unable to find file \"%s\"\n", filePath);
        return NULL;
    }

    int max_line_count = 32;
    // Initializing the programData object
    programData *instrList   = malloc(sizeof(programData));
    instrList->instructions  = malloc(max_line_count * sizeof(instructionLine *));
    instrList->nbInstruction = 0;
    instrList->pc            = 0;

    char buffer[COMPILED_LINE_SIZE+1];
    // Read all lines
    while (fgets(buffer, COMPILED_LINE_SIZE+1, file)) {
        // If more memory is needed, double memory allocation
        if (instrList->nbInstruction >= max_line_count) {
            max_line_count *= 2;
            instrList->instructions = realloc(instrList->instructions, max_line_count * sizeof(instructionLine *));
        }
        // Read the line
        instrList->instructions[instrList->nbInstruction] = readCompiledLine(buffer);
        instrList->nbInstruction++;
    }

    return instrList;
}


process *newProcess(char *filePath, int memorySize) {
    process *process = malloc(sizeof(process));
    process->memory  = newMemoryRegistry(memorySize);
    process->program = readCompiledFile(filePath);
    return process;
}


short *getRegistry(process *process, int number) {
    // Invalid registry number
    if (0 > number || number > process->memory->size-1) {
        printf("ERROR: INVALID REGISTRY ACCESS\n");
        printf("       On line %d, cannot access registry %d\n", process->program->pc+1, number);
        printf("       Can only access registries 0 to %d\n", process->memory->size-1);
        return NULL;
    }
    // Return a pointer to the registry, allowing it to be modified
    return &process->memory->registry[number];
}


int setRegistry(process *process, int number, short value) {
    // Get the registry
    short *registry = getRegistry(process, number);
    // Do nothing if the registry was not found
    if (!(registry)) return 0;
    // Set the registry to its new value
    (*registry) = value;
    return 1;
}


short *popRegistry(process *process) {
    process->memory->sp--;
    return getRegistry(process, process->memory->sp);
}


int pushRegistry(process *process, short value) {
    process->memory->sp++;
    return setRegistry(process, process->memory->sp-1, value);
}


int runProcess(process *process) {
    // Initializing random seed
    srand(time(NULL));
    // Temporary variables
    short *regA;
    short *regB;
    short  res;
    // Run the process
    instructionLine *line;
    while (1) {
        // Check if the program has a next instruction to run, otherwise throw an error
        if (0 <= process->program->pc && process->program->pc < process->program->nbInstruction) line = process->program->instructions[process->program->pc];
        else {
            printf("ERROR: MISSING NEXT INSTRUCTION\n");
            printf("       Reached line %d, but no instruction is present\n", process->program->pc+1);
            printf("       Instructions are writen on lines 1 to %d\n", process->program->nbInstruction);
            return 0;
        }

        // Instruction (00) pop x
        if      (line->instruction ==  0) {
            regA = popRegistry(process);                                  if (!(regA)) return 0;  // [Getting (SP-1) value] ; SP--
            res  = setRegistry(process, line->parameter, (*regA));        if (!(res))  return 0;  // (x) = (SP-1)
        }
        // Instruction (01) ipop
        else if (line->instruction ==  1) {
            regA = popRegistry(process);                                  if (!(regA)) return 0;  // [Getting (SP-1) value] ; SP--
            regB = popRegistry(process);                                  if (!(regB)) return 0;  // [Getting (SP-2) value] ; SP--
            res  = setRegistry(process, (*regA), (*regB));                if (!(res))  return 0;  // ((SP-1)) = (SP-2)
        }
        // Instruction (02) push x
        else if (line->instruction ==  2) {
            regA = getRegistry(process, line->parameter);                 if (!(regA)) return 0;  // [Getting (x) value]
            res  = pushRegistry(process, (*regA));                        if (!(res))  return 0;  // (SP) = (x) ; SP++
        }
        // Instruction (03) ipush
        else if (line->instruction ==  3) {
            regA = popRegistry(process);                                  if (!(regA)) return 0;  // [Getting (SP-1) value] ; SP--
            regB = getRegistry(process, (*regA));                         if (!(regB)) return 0;  // [Getting ((SP-1)) value]
            res  = pushRegistry(process, (*regB));                        if (!(res))  return 0;  // (SP-1) = ((SP-1)) ; SP++
        }
        // Instruction (04) push# i
        else if (line->instruction ==  4) {
            res  = pushRegistry(process, line->parameter);                if (!(res))  return 0;  // (SP) = i ; SP++
        }
        // Instruction (05) jmp adr
        else if (line->instruction ==  5) {
            process->program->pc += line->parameter;                                              // PC = PC + adr
        }
        // Instruction (06) jnz adr
        else if (line->instruction ==  6) {
            regA = popRegistry(process);                                  if (!(regA)) return 0;  // [Getting (SP-1) value] ; SP--
            if ((*regA)) process->program->pc += line->parameter;                                 // If (SP-1) ≠ 0 then PC = PC + adr
        }
        // Instruction (07) call adr
        else if (line->instruction ==  7) {
            res  = pushRegistry(process, process->program->pc);           if (!(res))  return 0;  // (SP) = PC ; SP++
            process->program->pc += line->parameter;                                              // PC = PC + adr
        }
        // Instruction (08) ret
        else if (line->instruction ==  8) {
            regA = popRegistry(process);                                  if (!(regA)) return 0;  // [Getting (SP-1) value] ; SP--
            process->program->pc = (*regA);                                                       // PC = (SP-1)
        }
        // Instruction (09) read x
        else if (line->instruction ==  9) {
            if (scanf("%hd", &res) != 1)                                               return 0;  // [Getting INPUT value]
            while (getchar() != '\n');                                                            // Clearing buffer
            res  = setRegistry(process, line->parameter, res);            if (!(res))  return 0;  // (x) = INPUT
        }
        // Instruction (10) write x
        else if (line->instruction == 10) {
            regA = getRegistry(process, line->parameter);                 if (!(regA)) return 0;  // [Getting (x) value]
            printf("%hd\n", (*regA));                                                             // OUTPUT = (x)
        }

        // Instruction (11) op i
        else if (line->instruction == 11) {
            // Base instruction for all possible operations (load used values)
            if (line->parameter != 9 && line->parameter != 15) {
                regB = popRegistry(process);                              if (!(regB)) return 0;  // [Getting (SP-1) value] ; SP--
                regA = popRegistry(process);                              if (!(regA)) return 0;  // [Getting (SP-2) value] ; SP--
            }
            else {
                regA = popRegistry(process);                              if (!(regA)) return 0;  // [Getting (SP-1) value] ; SP--
            }
            // Operation (00) "EQUAL"
            if      (line->parameter ==  0) {
                if ((*regA) == (*regB)) {res = pushRegistry(process, 1);  if (!(res))  return 0;} // if (SP-2) = (SP-1) then (SP-2) = 1 and SP++
                else                    {res = pushRegistry(process, 0);  if (!(res))  return 0;} // if (SP-2) ≠ (SP-1) then (SP-2) = 0 and SP++
            }
            // Operation (01) "DIFFERENT"
            else if (line->parameter ==  1) {
                if ((*regA) != (*regB)) {res = pushRegistry(process, 1);  if (!(res))  return 0;} // if (SP-2) ≠ (SP-1) then (SP-2) = 1 and SP++
                else                    {res = pushRegistry(process, 0);  if (!(res))  return 0;} // if (SP-2) = (SP-1) then (SP-2) = 0 and SP++
            }
            // Operation (02) "GREATER OR EQUAL"
            else if (line->parameter ==  2) {
                if ((*regA) >= (*regB)) {res = pushRegistry(process, 1);  if (!(res))  return 0;} // if (SP-2) ≥ (SP-1) then (SP-2) = 1 and SP++
                else                    {res = pushRegistry(process, 0);  if (!(res))  return 0;} // if (SP-2) < (SP-1) then (SP-2) = 0 and SP++
            }
            // Operation (03) "SMALLER OR EQUAL"
            else if (line->parameter ==  3) {
                if ((*regA) <= (*regB)) {res = pushRegistry(process, 1);  if (!(res))  return 0;} // if (SP-2) ≤ (SP-1) then (SP-2) = 1 and SP++
                else                    {res = pushRegistry(process, 0);  if (!(res))  return 0;} // if (SP-2) > (SP-1) then (SP-2) = 0 and SP++
            }
            // Operation (04) "STRICTLY GREATER"
            else if (line->parameter ==  4) {
                if ((*regA) >  (*regB)) {res = pushRegistry(process, 1);  if (!(res))  return 0;} // if (SP-2) > (SP-1) then (SP-2) = 1 and SP++
                else                    {res = pushRegistry(process, 0);  if (!(res))  return 0;} // if (SP-2) ≤ (SP-1) then (SP-2) = 0 and SP++
            }
            // Operation (05) "STRICTLY SMALLER"
            else if (line->parameter ==  5) {
                if ((*regA) <  (*regB)) {res = pushRegistry(process, 1);  if (!(res))  return 0;} // if (SP-2) < (SP-1) then (SP-2) = 1 and SP++
                else                    {res = pushRegistry(process, 0);  if (!(res))  return 0;} // if (SP-2) ≥ (SP-1) then (SP-2) = 0 and SP++
            }
            // Operation (06) "LOGICAL OR"
            else if (line->parameter ==  6) {
                res  = pushRegistry(process, (*regA) | (*regB));          if (!(res))  return 0;  // (SP-2) = (SP-2) | (SP-1) ; SP++
            }
            // Operation (07) "LOGICAL OR"
            else if (line->parameter ==  7) {
                res  = pushRegistry(process, (*regA) ^ (*regB));          if (!(res))  return 0;  // (SP-2) = (SP-2) ^ (SP-1) ; SP++
            }
            // Operation (08) "LOGICAL AND"
            else if (line->parameter ==  8) {
                res  = pushRegistry(process, (*regA) & (*regB));          if (!(res))  return 0;  // (SP-2) = (SP-2) & (SP-1) ; SP++
            }
            // Operation (09) "LOGICAL NOT"
            else if (line->parameter ==  9) {
                res  = pushRegistry(process, ~(*regA));                   if (!(res))  return 0;  // (SP-1) = ~(SP-1) ; SP++
            }
            // Operation (10) "ADD"
            else if (line->parameter == 10) {
                res  = pushRegistry(process, (*regA) + (*regB));          if (!(res))  return 0;  // (SP-2) = (SP-2) + (SP-1) ; SP++
            }
            // Operation (11) "SUBSTRACT"
            else if (line->parameter == 11) {
                res  = pushRegistry(process, (*regA) - (*regB));          if (!(res))  return 0;  // (SP-2) = (SP-2) - (SP-1) ; SP++
            }
            // Operation (12) "MULTIPLY"
            else if (line->parameter == 12) {
                res  = pushRegistry(process, (*regA) * (*regB));          if (!(res))  return 0;  // (SP-2) = (SP-2) * (SP-1) ; SP++
            }
            // Operation (13) "DIVIDE"
            else if (line->parameter == 13) {
                res  = pushRegistry(process, (*regA) / (*regB));          if (!(res))  return 0;  // (SP-2) = (SP-2) / (SP-1) ; SP++
            }
            // Operation (14) "MODULO"
            else if (line->parameter == 14) {
                res  = pushRegistry(process, (*regA) % (*regB));          if (!(res))  return 0;  // (SP-2) = (SP-2) % (SP-1) ; SP++
            }
            // Operation (15) "INVERT"
            else if (line->parameter == 15) {
                res  = pushRegistry(process, -(*regA));                   if (!(res))  return 0;  // (SP-1) = -(SP-1) ; SP++
            }
            // Invalid operation
            else {
                printf("ERROR: INVALID OPERATION CODE\n");
                printf("       On line %d, operation code %d do not exist\n", process->program->pc+1, line->parameter);
                printf("       Valid instruction codes are 0-15\n");
                return 0;
            }
        }

        // Instruction (12) rnd i
        else if (line->instruction == 12) {
            res  = pushRegistry(process, rand() % line->parameter);       if (!(res))  return 0;  // (SP) = RANDOM % x ; SP++
        }
        // Instruction (13) dup
        else if (line->instruction == 13) {
            regA = popRegistry(process);                                  if (!(regA)) return 0;  // [Getting (SP-1) value] ; SP--
            res  = pushRegistry(process, (*regA));                        if (!(res))  return 0;  // (SP-1) = (SP-1) ; SP++
            res  = pushRegistry(process, (*regA));                        if (!(res))  return 0;  // (SP) = (SP-1) ; SP++
        }
        // Instruction (99) halt
        else if (line->instruction == 99) {
            return 1;                                                                                       // EXIT
        }
        // Invalid instruction
        else {
            printf("ERROR: INVALID INSTRUCTION CODE\n");
            printf("       On line %d, instruction code %d do not exist\n", process->program->pc+1, line->instruction);
            printf("       Valid instruction codes are 0-13 and 99\n");
            return 0;
        }
        // Go to the next instruction
        process->program->pc++;
    }

    return 1;
}


void killProcess(process *process) {
    if (!(process)) return;

    // Unload program
    if (process->program) {
        // Unload all instructions
        if (process->program->instructions) {
            // TODO!
            // This still may cause some random crashes...
            // for (int i = 0; i < process->program->nbInstruction; i++) if (process->program->instructions[i]) free(process->program->instructions[i]);
            free(process->program->instructions);
        }
        free(process->program);
    }

    // Free memory
    if (process->memory) {
        if (process->memory->registry) free(process->memory->registry);
        free(process->memory);
    }

    free(process);
}


int executeProcess(char *filePath, int memory_size) {
    // Load process
    process *process = newProcess(filePath, memory_size);
    if (!(process)) return 0;

    // Run process
    if (!(runProcess(process))) {
        printf("ERROR: RUNTIME ERROR\n");
        printf("       Process exited early due to an error\n");
        // Kill process to free memory used
        killProcess(process);
        return 0;
    }

    // Kill process to free memory used
    killProcess(process);
    return 1;
}
