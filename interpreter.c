#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instructions.h"
#include "parser.h"

void print_instruction(Instruction instruction, int label)
{
    if(instruction.type == Inc)
    {
        IncrementInstruction inc = instruction.instruction.inc;
        printf("L%u : R%u+ -> L%u\n", label, inc.reg, inc.jmp_lbl);
    }
    else if(instruction.type == Dec)
    {
        DecrementInstruction dec = instruction.instruction.dec;
        printf("L%u : R%u- -> L%u,L%u\n", label, dec.reg, dec.jmp_lbl_not_zero, dec.jmp_lbl_zero);
    }
    else if(instruction.type == Halt)
    {
        printf("L%u : HALT\n", label);
    }
    else
    {
        //Unrecognised
    }
}

//Returns address of next instruction, -1 if a proper halt occurs, -2 if an out of bounds register is accessed
int step(int registersc, int registersv[], int label, Instruction instructions[], int trace)
{
    Instruction current_instruction = instructions[label];
    
    if(trace)
    {
        print_instruction(current_instruction, label);
    }
    
    if(current_instruction.type == Inc)
    {
        IncrementInstruction inc = current_instruction.instruction.inc;
        
        if(inc.reg < registersc)
        {
            registersv[inc.reg]++;
            return inc.jmp_lbl;
        }
        else
        {
            //error : register out of bounds
            return -2;
        }       
    }
    else if(current_instruction.type == Dec)
    {
        DecrementInstruction dec = current_instruction.instruction.dec;

        if(dec.reg < registersc)
        {
            if(registersv[dec.reg] > 0)
            {
                registersv[dec.reg]--;
                return dec.jmp_lbl_not_zero;
            }
            else
            {
                return dec.jmp_lbl_zero;
            }
        }
        else
        {
            //error : register out of bounds
            return -2;
        }
    }
    else if(current_instruction.type == Halt)
    {
        return -1;
    }
    else
    {
        return -3; //Unrecognised instruction (should never happen)
    }
}

void run(int registersc, int registersv[], int instructionsc, Instruction instructionsv[], int trace)
{
    unsigned int label = 0;
    
    while(1)
    {
        label = step(registersc, registersv, label, instructionsv, trace);
        if(label == -2) //Register out of bounds
        {
            fprintf(stderr, "Register out of bounds, halting");
            return;
        }
        if(label == -1) //Proper halt
        {
            printf("Proper halt occured\n");
            return;
        }
        else if(label >= instructionsc) //Improper halt
        {
            printf("Improper halt occured\n");
            return;
        }
    }
}

int main(int argc, char *argv[])
{
    Instruction* instructionsv;
    int instructionsc = parse_stdin(&instructionsv);
    
    printf("Instructions:\n");
    for(int i = 0; i < instructionsc; i++)
    {
        print_instruction(instructionsv[i], i);
    }
    
    int registers[] = { 0, 1, 2 };
    
    printf("Register starting state:\n");
    for(int i = 0; i < 3; i++)
    {
        printf("R%u : %u, ", i, registers[i]);
    }
    printf("\n");
    
    run(3, registers, instructionsc, instructionsv, 1);
    
    printf("Register ending state:\n");
    for(int i = 0; i < 3; i++)
    {
        printf("R%u : %u, ", i, registers[i]);
    }
    printf("\n");
    
    free(instructionsv);
    
    return 0;
}
