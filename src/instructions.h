#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

typedef struct
{
    unsigned int reg;
    unsigned int jmp_lbl;
} IncrementInstruction;

typedef struct
{
    unsigned int reg;
    unsigned int jmp_lbl_not_zero;
    unsigned int jmp_lbl_zero;
} DecrementInstruction;

typedef struct
{

} HaltInstruction;

typedef enum
{
    Inc,
    Dec,
    Halt
} InstructionType;

typedef struct
{
    InstructionType type;
    union
    {
        IncrementInstruction inc;
        DecrementInstruction dec;
        HaltInstruction halt;
    } instruction;
} Instruction;

void instruction_to_string(const Instruction instruction);

#endif
