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


memoryRegistry *newMemoryRegistry(size_t size) {
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

    size_t max_line_count = 32;
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


process *newProcess(char *filePath, size_t memorySize) {
    process *process = malloc(sizeof(process));
    process->memory  = newMemoryRegistry(memorySize);
    process->program = readCompiledFile(filePath);
    return process;
}


short *getRegistry(process *process, long long number) {
    // Invalid registry number
    if (0 > number || (size_t) number > process->memory->size-1) {
        printf("ERROR: INVALID REGISTRY ACCESS\n");
        printf("       On line %lld, cannot access registry %lld\n", process->program->pc+1, number);
        printf("       Can only access registries 0 to %lld\n", process->memory->size-1);
        return NULL;
    }
    // Return a pointer to the registry, allowing it to be modified
    short *registry = &process->memory->registry[number];
    return registry;
}


int setRegistry(process *process, size_t number, short value) {
    // Get the registry
    short *registry = getRegistry(process, number);
    // Do nothing if the registry was not found
    if (!(registry)) return 0;
    // Set the registry to its new value
    (*registry) = value;
    return 1;
}


int runProcess(process *process) {
    // Temporary variables
    short *regA;
    short *regB;
    short  res;
    // Run the process
    instructionLine *line;
    while (1) {
        // Check if the program has a next instruction to run, otherwise throw an error
        if (0 <= process->program->pc && process->program->pc < (long long) process->program->nbInstruction) line = process->program->instructions[process->program->pc];
        else {
            printf("ERROR: MISSING NEXT INSTRUCTION\n");
            printf("       Reached line %lld, but no instruction is present\n", process->program->pc+1);
            printf("       Instructions are writen on lines 1 to %lld\n", process->program->nbInstruction);
            return 0;
        }

        // Instruction (00) pop x
        if      (line->instruction ==  0) {
            // [SP-- ; (x)=(SP)]
            process->memory->sp--;                                                                          // SP--
            regA = getRegistry(process, process->memory->sp);                       if (!(regA)) return 0;  // [Getting (SP) value]
            res  = setRegistry(process, line->parameter, (*regA));                  if (!(res))  return 0;  // (x) = (SP)
        }
        // Instruction (01) ipop
        else if (line->instruction ==  1) {
            // [((SP-1))=(SP-2) ; SP=SP-2]
            regA = getRegistry(process, process->memory->sp-1);                     if (!(regA)) return 0;  // [Getting (SP-1) value]
            regB = getRegistry(process, process->memory->sp-2);                     if (!(regB)) return 0;  // [Getting (SP-2) value]
            res  = setRegistry(process, (*regA), (*regB));                          if (!(res))  return 0;  // ((SP-1)) = (SP-2)
            process->memory->sp -= 2;                                                                       // SP = SP - 2
        }
        // Instruction (02) push x
        else if (line->instruction ==  2) {
            // [(SP)=(x) ; SP++]
            regA = getRegistry(process, line->parameter);                           if (!(regA)) return 0;  // [Getting (x) value]
            res  = setRegistry(process, process->memory->sp, (*regA));              if (!(res))  return 0;  // (SP) = (x)
            process->memory->sp++;                                                                          // SP++
        }
        // Instruction (03) ipush
        else if (line->instruction ==  3) {
            // [(SP-1)=((SP-1))]
            regA = getRegistry(process, process->memory->sp-1);                     if (!(regA)) return 0;  // [Getting (SP-1) value]
            regB = getRegistry(process, (*regA));                                   if (!(regB)) return 0;  // [Getting ((SP-1)) value]
            (*regA) = (*regB);                                                                              // (SP-1) = ((SP-1))
        }
        // Instruction (04) push# i
        else if (line->instruction ==  4) {
            // [(SP)=i ; SP++]
            res  = setRegistry(process, process->memory->sp, line->parameter);      if (!(res))  return 0;  // (SP) = i
            process->memory->sp++;                                                                          // SP++
        }
        // Instruction (05) jmp adr
        else if (line->instruction ==  5) {
            // [PC=PC+adr]
            process->program->pc += line->parameter;                                                        // PC = PC + adr
        }
        // Instruction (06) jnz adr
        else if (line->instruction ==  6) {
            // [SP-- ; If (SP)≠0 then PC=PC+adr]
            process->memory->sp--;                                                                          // SP--
            regA = getRegistry(process, process->memory->sp);                       if (!(regA)) return 0;  // [Getting (SP) value]
            if ((*regA)) process->program->pc += line->parameter;                                           // If (SP) ≠ 0 then PC = PC + adr
        }
        // Instruction (07) call adr
        else if (line->instruction ==  7) {
            // [(SP)=PC ; SP++ ; PC=PC+adr]
            res  = setRegistry(process, process->memory->sp, process->program->pc); if (!(res))  return 0;  // (SP) = PC
            process->memory->sp++;                                                                          // SP++
            process->program->pc += line->parameter;                                                        // PC = PC + adr
        }
        // Instruction (08) ret
        else if (line->instruction ==  8) {
            // [SP-- ; PC=(SP)]
            process->memory->sp--;                                                                          // SP--
            regA = getRegistry(process, process->memory->sp);                       if (!(regA)) return 0;  // [Getting (SP) value]
            process->program->pc = (*regA);                                                                 // PC = (SP)
        }
        // Instruction (09) read x
        else if (line->instruction ==  9) {
            // [(x)=INPUT]
            if (scanf("%hd", &res) != 1)                                                         return 0;  // [Getting INPUT value]
            while (getchar() != '\n');                                                                      // Clearing buffer
            res  = setRegistry(process, line->parameter, res);                      if (!(res))  return 0;  // (x) = INPUT
        }
        // Instruction (10) write x
        else if (line->instruction == 10) {
            // [OUTPUT=(x)]
            regA = getRegistry(process, line->parameter);                           if (!(regA)) return 0;  // [Getting (x) value]
            printf("%hd\n", (*regA));                                                                       // OUTPUT = (x)
        }

        // Instruction (11) op i
        else if (line->instruction == 11) {
            // Base instruction for all possible operations (load used values)
            if (line->parameter != 9 && line->parameter != 15) {
                // [SP--]
                process->memory->sp--;                                                                      // SP--
                regA = getRegistry(process, process->memory->sp-1);                 if (!(regA)) return 0;  // [Getting (SP-1) value]
                regB = getRegistry(process, process->memory->sp);                   if (!(regB)) return 0;  // [Getting (SP) value]
            }
            else {
                regA = getRegistry(process, process->memory->sp-1);                 if (!(regA)) return 0;  // [Getting (SP-1) value]
            }
            // Operation (00) "EQUAL"
            if      (line->parameter ==  0) {
                // [SP-- ; if (SP-1)=(SP) then (SP-1)=1 else (SP-1)=0]
                if ((*regA) == (*regB)) (*regA) = 1;                                                        // if (SP-1) = (SP) then (SP-1) = 1
                else                    (*regA) = 0;                                                        // if (SP-1) ≠ (SP) then (SP-1) = 0
            }
            // Operation (01) "DIFFERENT"
            else if (line->parameter ==  1) {
                // [SP-- ; if (SP-1)≠(SP) then (SP-1)=1 else (SP-1)=0]
                if ((*regA) != (*regB)) (*regA) = 1;                                                        // if (SP-1) ≠ (SP) then (SP-1) = 1
                else                    (*regA) = 0;                                                        // if (SP-1) = (SP) then (SP-1) = 0
            }
            // Operation (02) "GREATER OR EQUAL"
            else if (line->parameter ==  2) {
                // [SP-- ; if (SP-1)≥(SP) then (SP-1)=1 else (SP-1)=0]
                if ((*regA) >= (*regB)) (*regA) = 1;                                                        // if (SP-1) ≥ (SP) then (SP-1) = 1
                else                    (*regA) = 0;                                                        // if (SP-1) < (SP) then (SP-1) = 0
            }
            // Operation (03) "SMALLER OR EQUAL"
            else if (line->parameter ==  3) {
                // [SP-- ; if (SP-1)≤(SP) then (SP-1)=1 else (SP-1)=0]
                if ((*regA) <= (*regB)) (*regA) = 1;                                                        // if (SP-1) ≤ (SP) then (SP-1) = 1
                else                    (*regA) = 0;                                                        // if (SP-1) > (SP) then (SP-1) = 0
            }
            // Operation (04) "STRICTLY GREATER"
            else if (line->parameter ==  4) {
                // [SP-- ; if (SP-1)>(SP) then (SP-1)=1 else (SP-1)=0]
                if ((*regA) >  (*regB)) (*regA) = 1;                                                        // if (SP-1) > (SP) then (SP-1) = 1
                else                    (*regA) = 0;                                                        // if (SP-1) ≤ (SP) then (SP-1) = 0
            }
            // Operation (05) "STRICTLY SMALLER"
            else if (line->parameter ==  5) {
                // [SP-- ; if (SP-1)<(SP) then (SP-1)=1 else (SP-1)=0]
                if ((*regA) <  (*regB)) (*regA) = 1;                                                        // if (SP-1) < (SP) then (SP-1) = 1
                else                    (*regA) = 0;                                                        // if (SP-1) ≥ (SP) then (SP-1) = 0
            }
            // Operation (06) "LOGICAL OR"
            else if (line->parameter ==  6) {
                // [SP-- ; (SP-1)=(SP-1)|(SP)]
                (*regA) = (*regA) | (*regB);                                                                // (SP-1) = (SP-1) | (SP)
            }
            // Operation (07) "LOGICAL OR"
            else if (line->parameter ==  7) {
                // [SP-- ; (SP-1)=(SP-1)^(SP)]
                (*regA) = (*regA) ^ (*regB);                                                                // (SP-1) = (SP-1) ^ (SP)
            }
            // Operation (08) "LOGICAL AND"
            else if (line->parameter ==  8) {
                // [SP-- ; (SP-1)=(SP-1)&(SP)]
                (*regA) = (*regA) & (*regB);                                                                // (SP-1) = (SP-1) & (SP)
            }
            // Operation (09) "LOGICAL NOT"
            else if (line->parameter ==  9) {
                // [SP-- ; (SP-1)=~(SP-1)]
                (*regA) = ~(*regA);                                                                         // (SP-1) = ~(SP - 1)
            }
            // Operation (10) "ADD"
            else if (line->parameter == 10) {
                // [SP-- ; (SP-1)=(SP-1)+(SP)]
                (*regA) = (*regA) + (*regB);                                                                // (SP-1) = (SP-1) + (SP)
            }
            // Operation (11) "SUBSTRACT"
            else if (line->parameter == 11) {
                // [SP-- ; (SP-1)=(SP-1)-(SP)]
                (*regA) = (*regA) - (*regB);                                                                // (SP-1) = (SP-1) - (SP)
            }
            // Operation (12) "MULTIPLY"
            else if (line->parameter == 12) {
                // [SP-- ; (SP-1)=(SP-1)*(SP)]
                (*regA) = (*regA) * (*regB);                                                                // (SP-1) = (SP-1) * (SP)
            }
            // Operation (13) "DIVIDE"
            else if (line->parameter == 13) {
                // [SP-- ; (SP-1)=(SP-1)/(SP)]
                (*regA) = (*regA) / (*regB);                                                                // (SP-1) = (SP-1) / (SP)
            }
            // Operation (14) "MODULO"
            else if (line->parameter == 14) {
                // [SP-- ; (SP-1)=(SP-1)%(SP)]
                (*regA) = (*regA) % (*regB);                                                                // (SP-1) = (SP-1) % (SP)
            }
            // Operation (15) "INVERT"
            else if (line->parameter == 15) {
                // [SP-- ; (SP-1)=-(SP-1)]
                (*regA) = -(*regA);                                                                         // (SP-1) = -(SP-1)
            }
            // Invalid operation
            else {
                printf("ERROR: INVALID OPERATION CODE\n");
                printf("       On line %lld, operation code %d do not exist\n", process->program->pc+1, line->parameter);
                printf("       Valid instruction codes are 0-15\n");
                return 0;
            }
        }

        // Instruction (12) rnd x
        // TODO!
        // Instruction (13) dup
        else if (line->instruction == 13) {
            // [(SP)=(SP-1) ; SP++]
            regA = getRegistry(process, process->memory->sp-1);                     if (!(regA)) return 0;  // [Getting (SP-1) value]
            res  = setRegistry(process, process->memory->sp, (*regA));              if (!(res))  return 0;  // (SP-1) = ((SP-1))
            process->memory->sp++;                                                                          // SP++
        }
        // Instruction (99) halt
        else if (line->instruction == 99) {
            // [EXIT]
            return 1;                                                                                       // EXIT
        }
        // Invalid instruction
        else {
            printf("ERROR: INVALID INSTRUCTION CODE\n");
            printf("       On line %lld, instruction code %d do not exist\n", process->program->pc+1, line->instruction);
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
            // For some reason, the line bellow causes a lot of issues (crash), but I have absolutly no clue why :(
            // for (size_t i = 0; i < process->program->nbInstruction; i++) if (process->program->instructions[i]) free(process->program->instructions[i]);
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


int executeProcess(char *filePath, size_t memory_size) {
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
