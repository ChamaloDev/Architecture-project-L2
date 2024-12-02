/*
    This is the main file of the program.
    To compile it, eather run:
        - "compile_on_Windows.cmd" if you are using Windows
        - "compile_on_Linux.sh"    if you are using Linux
    The program can then be runned by executing the following command:
        "./simulateur <inputPath> [<outputPath>]"
*/


#include <stdio.h>
#include "src/assembly.h"
#include "src/simulator.h"


// argCount: Number of argument passed
// argValue: Array of string, of size argCount
// The first argument is the input file repertory
// The second optionnal argument is the output file repertory
int main(int argCount, char **argValue) {
    // Ignore first argument
    argCount -= 1;
    argValue += sizeof(char);
    // The program needs 1 or 2 arguments, otherwise abort the process
    if (argCount != 1 && argCount != 2) {
        printf("ERROR: Expected 1 or 2 arguments, got %d\n", argCount);
        printf("ERROR: Unable to complete process\n");
        return 1;
    }
    // If the right number of arguments is passed, continue
    char *inputPath  = argValue[0];
    char *outputPath = (argCount == 2) ? argValue[1] : "hexa.txt";
    // Assembling instructions, abort process if unsuccessful in doing so
    if (!(assemble(inputPath, outputPath))) {
        printf("ERROR: Unable to generate output file\n");
        printf("ERROR: Unable to complete process\n");
        return 1;
    }
    // If the program was able to assemble the instructions, continue
    // TODO!
    return 0;
}
