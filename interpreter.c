#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct increment_instruction_t
{
    unsigned int reg;
    unsigned int jmp_lbl;
} IncrementInstruction;

typedef struct decrement_instruction_t
{
    unsigned int reg;
    unsigned int jmp_lbl_not_zero;
    unsigned int jmp_lbl_zero;
} DecrementInstruction;

typedef struct halt_instruction_t
{

} HaltInstruction;

typedef enum instruction_type_e
{
    Inc,
    Dec,
    Halt
} InstructionType;

typedef struct instruction_t
{
    InstructionType type;
    union instruction_u
    {
        IncrementInstruction inc;
        DecrementInstruction dec;
        HaltInstruction halt;
    } instruction;
} Instruction;

//Returns address of next instruction, or -1 if a proper halt occurs
int step(int registersc, int registersv[], int label, Instruction instructions[], int trace)
{
    Instruction current_instruction = instructions[label];
    
    if(current_instruction.type == Inc)
    {
        IncrementInstruction inc = current_instruction.instruction.inc;
        if(trace)
        {
            printf("L%u : R%u+ -> L%u\n", label, inc.reg, inc.jmp_lbl);
        }
        
        if(inc.reg < registersc)
        {
            registersv[inc.reg]++;
        }
        else
        {
            //error : register out of bounds
        }
        return inc.jmp_lbl;        
    }
    else if(current_instruction.type == Dec)
    {
        DecrementInstruction dec = current_instruction.instruction.dec;
        if(trace)
        {
            printf("L%u : R%u- -> L%u,L%u\n", label, dec.reg, dec.jmp_lbl_not_zero, dec.jmp_lbl_zero);
        }
        
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
        }
    }
    else if(current_instruction.type == Halt)
    {
        if(trace)
        {
            printf("L%u : HALT\n", label);
        }
        return -1;
    }
}

void run(int registersc, int registersv[], int instructionsc, Instruction instructionsv[], int trace)
{
    unsigned int label = 0;
    
    while(1)
    {
        label = step(registersc, registersv, label, instructionsv, trace);
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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    
    //while((read = getline(&line, &len, stdin)) != -1)
    //{
    //    printf("Line of length %zu : ", read);
    //    printf("%s", line);
    //}
    
    //free(line);
    
    Instruction instructions[] = 
    {
        { .type=Dec, .instruction= { .dec= { .reg = 1, .jmp_lbl_not_zero = 1, .jmp_lbl_zero = 2 } } },
        { .type=Inc, .instruction= { .inc= { .reg = 0, .jmp_lbl = 0 } } },
        { .type=Dec, .instruction= { .dec= { .reg = 2, .jmp_lbl_not_zero = 3, .jmp_lbl_zero = 4 } } },
        { .type=Inc, .instruction= { .inc= { .reg = 0, .jmp_lbl = 2 } } },
        { .type=Halt, .instruction= { .halt= {  } } }
    };
    
    int registers[] = { 0, 1, 2 };
    
    printf("Register starting state:\n");
    for(int i = 0; i < 3; i++)
    {
        printf("R%u : %u, ", i, registers[i]);
    }
    printf("\n");
    
    run(3, registers, 5, instructions, 1);
    
    printf("Register ending state:\n");
    for(int i = 0; i < 3; i++)
    {
        printf("R%u : %u, ", i, registers[i]);
    }
    printf("\n");
    
    return 0;
}
