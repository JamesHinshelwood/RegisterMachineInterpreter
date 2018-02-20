#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
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

void print_registers(int registersc, int registersv[])
{
    printf("--------------------\nRegisters:\n");
    for(int i = 0; i < registersc; i++)
    {
        printf("R%u : %u", i, registersv[i]);
        if(i != registersc - 1)
            printf(", ");
    }
    printf("\n--------------------\n");
}

void print_instructions(int instructionsc, Instruction instructionsv[])
{
    printf("--------------------\nInstructions:\n");
    for(int i = 0; i < instructionsc; i++)
    {
        print_instruction(instructionsv[i], i);
    }
    printf("--------------------\n");
}

//Returns address of next instruction, -1 if a proper halt occurs, -2 if an out of bounds register is accessed
int step(int registersc, int registersv[], int label, Instruction instructions[], int verbose)
{
    Instruction current_instruction = instructions[label];
    
    if(verbose)
        print_instruction(current_instruction, label);
    
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

void run(int registersc, int registersv[], int instructionsc, Instruction instructionsv[], int verbose)
{
    unsigned int label = 0;
    
    while(1)
    {
        label = step(registersc, registersv, label, instructionsv, verbose);
        if(label == -2) //Register out of bounds
        {
            fprintf(stderr, "Register out of bounds, halting\n");
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
    int verbose = 0;    
    
    int c;
    while((c = getopt(argc, argv, "v")) != -1)
    {
        switch(c)
        {
            case 'v':
                verbose = 1;
                break;
            case '?':
                return 1;
            default:
                abort();
        }
    }

    int registersc = argc - optind;
    if(registersc == 0)
    {
        fprintf(stderr, "No registers specified in program arguments\n");
        return 1;
    }
    int r[registersc + 5]; // 5 spare registers for scratch space
    int* registersv = r;
    registersv += 5;
    for(int i = 0; i < 5; i++)
    {
        registersv[0-i] = 0;
    }
    for(int i = 0; i < registersc; i++)
    {
        registersv[i] = atoi(argv[optind+i]);
    }
    
    Instruction* instructionsv;
    int instructionsc = parse_stdin(&instructionsv);
    
    print_instructions(instructionsc, instructionsv);
    
    print_registers(registersc, registersv);
    
    run(registersc, registersv, instructionsc, instructionsv, verbose);
    
    print_registers(registersc, registersv);
    
    free(instructionsv);
    
    return 0;
}
