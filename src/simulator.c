/*
    In this file are all elements used to simulate
    the execution of "machine language".
*/


#include "simulator.h"
#include <stdio.h>
#include <stdlib.h>




memoryRegistry *newMemoryRegistry() {
    memoryRegistry *m   = malloc(sizeof(memoryRegistry));
    m->registry         = malloc(sizeof(short) * 5000);
    m->sp               = 0;
    return m;
}


// TODO!
