/*
    Project by : Avihai Halioua & Térence de Saint Léger


    This is the main file of the program.

    To compile it, eather run:
        - "bash compile_on_Linux.sh" if you are using Linux terminal
        - "./compile_on_Windows.cmd" if you are using Windows terminal
        - "compile_on_Windows.cmd"   if you are using Windows command prompt
    The program can then be runned by executing the following command:
        - "./simulateur <inputPath> [<outputPath>]" if you are using Linux terminal or the Windows terminal
        - "simulateur <inputPath> [<outputPath>]"   if you are using the Windows command prompt

    <inputPath> correspond to the path to the assembly file
    <outputPath> correspond to the path to the compiled file, defaults to "hexa.txt"
*/




#include <stdio.h>
#include "src/compiler.h"
#include "src/simulator.h"




// argCount: Number of argument passed
// argValue: Array of string, of size argCount
// The first argument is the input file repertory
// The second optionnal argument is the output file repertory
int main(int argCount, char **argValue) {
    // Ignore first argument
    argCount -= 1;
    argValue += 1;
    // The program needs 1 or 2 arguments, otherwise abort the process
    if (argCount != 1 && argCount != 2) {
        printf("ERROR: INVALID ARGUMENT COUNT\n");
        printf("       Expected 1 or 2 arguments, got %d\n", argCount);
        printf("INFO:  Process FAILLED to complete\n");
        return 1;
    }

    char *inputPath  = argValue[0];
    char *outputPath = ((argCount == 2) ? (argValue[1]) : ("hexa.txt"));
    // Assembling instructions, throw error if unsuccessful in doing so
    if (!(assemble(inputPath, outputPath))) {
        printf("INFO:  Process FAILLED to complete\n");
        return 1;
    }

    /*
        TODO!
        Do simulation here
    */
    printf("INFO:  Process SUCCESSFULLY completed\n");
    return 0;
}
