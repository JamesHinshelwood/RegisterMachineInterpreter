#include <stdio.h>
#include "instructions.h"

void instruction_to_string(const Instruction instruction)
{
    if(instruction.type == Inc)
    {
        IncrementInstruction inc = instruction.instruction.inc;
        printf("INC %u %u\n", inc.reg, inc.jmp_lbl);
    }
    else if(instruction.type == Dec)
    {
        DecrementInstruction dec = instruction.instruction.dec;
        printf("DEC %u %u %u\n", dec.reg, dec.jmp_lbl_not_zero, dec.jmp_lbl_zero);
    }
    else if(instruction.type == Halt)
    {
        printf("HALT\n");
    }
    else
    {
        //Unrecognised
    }
}