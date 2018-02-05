#include <stdlib.h>
#include <stdio.h>
#include "instructions.h"
#include "instruction_array.h"

int array_init(InstructionArray* a, size_t initial_size)
{
    a->array = malloc(initial_size * sizeof(Instruction));
    if(a->array == NULL)
    {
        fprintf(stderr, "Memory allocation for instruction array failed");
        return 1;
    }
    a->used = 0;
    a->size = initial_size;
    return 0;
}

int array_add(InstructionArray* a, Instruction i)
{
    if(a->used == a->size)
    {
        a->size *= 2;
        a->array = realloc(a, a->size * sizeof(Instruction));
        if(a->array == NULL)
        {
            fprintf(stderr, "Resizing instruction array failed");
            return 1;
        }
    }
    a->array[a->used++] = i;
    return 0;
}

void array_free(InstructionArray* a)
{
    free(a->array);
    a->array = NULL;
    a->used = 0;
    a->size = 0;
}
