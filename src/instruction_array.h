#ifndef INSTRUCTION_ARRAY_H
#define INSTRUCTION_ARRAY_H
typedef struct
{
    Instruction* array;
    size_t used;
    size_t size;
} InstructionArray;

int array_init(InstructionArray* a, size_t initial_size);

int array_add(InstructionArray* a, Instruction i);

void array_free(InstructionArray* a);
#endif
