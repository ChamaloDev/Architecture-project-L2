/*
    In this file are all elements used to simulate
    the execution of "machine language".
*/


#include "simulator.h"
#include <stdio.h>
#include <stdlib.h>




memoryRegistry *newMemoryRegistry(long long size) {
    memoryRegistry *m = malloc(sizeof(memoryRegistry));
    m->registry       = malloc(size * sizeof(short));
    m->size           = size;
    m->sp             = 0;
    return m;
}


// TODO!
