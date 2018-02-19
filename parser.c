#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instructions.h"
#include "instruction_array.h"
#include "parser.h"

int parse_line(char* line, Instruction* instruction)
{
    char* saveptr;
    char* token = strtok_r(line, " ", &saveptr);

    if(token == NULL)
        return 1;
        
    if(strcmp(token, "HALT") == 0)
    {
        instruction->type = Halt;
        HaltInstruction halt = { };
        instruction->instruction.halt = halt;
        
        return 0;
    }
    else if(strcmp(token, "INC") == 0)
    {
        instruction->type = Inc;
        
        token = strtok_r(NULL, " ", &saveptr);
        instruction->instruction.inc.reg = atoi(token);
        
        token = strtok_r(NULL, " ", &saveptr);
        instruction->instruction.inc.jmp_lbl = atoi(token);
        
        return 0;
    }
    else if(strcmp(token, "DEC") == 0)
    {
        instruction->type = Dec;
        
        token = strtok_r(NULL, " ", &saveptr);
        instruction->instruction.dec.reg = atoi(token);
        
        token = strtok_r(NULL, " ", &saveptr);
        instruction->instruction.dec.jmp_lbl_not_zero = atoi(token);
        
        token = strtok_r(NULL, " ", &saveptr);
        instruction->instruction.dec.jmp_lbl_zero = atoi(token);
        
        return 0;
    }
    else
    {
        //Parsing error
        return 1;
    }
}

int parse_stdin(Instruction* instructions[])
{
    InstructionArray array;
    
    if(array_init(&array, 1) != 0)
        return -1;
    
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    
    while((read = getline(&line, &len, stdin)) != -1)
    {
        //Remove trailing \n from line
        line[strlen(line)-1] = '\0';
        
        Instruction instruction;
        parse_line(line, &instruction);
        if(array_add(&array, instruction) != 0)
            return -1;
    }
    
    free(line);
    
    *instructions = array.array;
    
    return array.used;
}
