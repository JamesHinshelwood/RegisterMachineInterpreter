#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gmp.h>
#include "instructions.h"
#include "instruction_array.h"
#include "parser.h"
#include "packer.h"

typedef struct
{
    mpz_t first;
    mpz_t second;
} mpz_t_pair;

void pair(mpz_t result, const mpz_t x, const mpz_t y)
{
    mpz_t a, b;
    mpz_inits(a, b, NULL);
    
    mpz_ui_pow_ui(a, 2, mpz_get_ui(x)); // a = 2 ^ x

    mpz_mul_2exp(b, y, 1); // b = y * 2
    mpz_add_ui(b, b, 1); // b++
    
    mpz_mul(result, a, b); // out = a * b
    
    mpz_clears(a, b, NULL);
}

void un_pair(mpz_t_pair* result, const mpz_t pair)
{
    mpz_t bitmask;
    mpz_init_set_ui(bitmask, 1);
    
    unsigned long x = 0;

    mp_bitcnt_t one_bit = mpz_scan1(pair, 0);
    
    x = one_bit;
    
    /*
    mpz_mul_2exp(bitmask, bitmask, 1); // bitmask =  bitmask << 1
    mpz_sub_ui(bitmask, bitmask, 1); // bitmask = bitmask - 1
    mpz_com(bitmask, bitmask); // bitmask = ~bitmask
    
    mpz_t y;
    mpz_init(y);
    mpz_and(y, bitmask, pair); // y = (pair & bitmask)
    */
    mpz_t y;
    mpz_init(y);
    mpz_tdiv_q_2exp(y, pair, x+1); // y = pair >> (x+1)
       
    mpz_init_set_ui(result->first, x);
    mpz_init_set(result->second, y);
    
    mpz_clears(bitmask, y, NULL);
}

void pair_minus_one(mpz_t result, const mpz_t x, const mpz_t y)
{
    pair(result, x, y);
    mpz_sub_ui(result, result, 1);
}

void un_pair_minus_one(mpz_t_pair* result, const mpz_t pair)
{
    mpz_t p;
    mpz_init_set(p, pair);
    
    mpz_add_ui(p, p, 1); // p++
    
    un_pair(result, p);
    
    mpz_clear(p);
}

void list_cons(mpz_t result, const mpz_t head, const mpz_t tail)
{
    pair(result, head, tail);
}

void list_split(mpz_t_pair* result, const mpz_t list)
{
    un_pair(result, list);
}

void encode_prog(mpz_t result, const int instructionsc, const Instruction* instructionsv)
{
    mpz_set_ui(result, 0);
    
    for(int i = instructionsc - 1; i >= 0; i--)
    {
        Instruction instruction = instructionsv[i];
        mpz_t body;
        mpz_init_set_ui(body, 0);
        
        if(instruction.type == Inc)
        {
            IncrementInstruction inc = instruction.instruction.inc;
            
            mpz_t x;
            mpz_init_set_ui(x, 2 * inc.reg);
            
            mpz_t y;
            mpz_init_set_ui(y, inc.jmp_lbl);
            
            pair(body, x, y);
            
            mpz_clears(x, y, NULL);
        }
        else if(instruction.type == Dec)
        {
            DecrementInstruction dec = instruction.instruction.dec;
            
            mpz_t x;
            mpz_init_set_ui(x, 2 * dec.reg + 1);
            
            mpz_t y;
            mpz_init_set_ui(y, dec.jmp_lbl_not_zero);
            
            mpz_t z;
            mpz_init_set_ui(z, dec.jmp_lbl_zero);
            
            mpz_t p;
            mpz_init(p);
            pair_minus_one(p, y, z);
            pair(body, x, p);
            
            mpz_clears(x, y, z, p, NULL);
        }
        else if(instruction.type == Halt)
        {
            mpz_set_ui(body, 0);
        }
        else
        {
            //Unrecognised
        }
        
        list_cons(result, body, result); // result = body::result
        
        mpz_clear(body);
    }
}

Instruction decode_instruction(const mpz_t body)
{
    Instruction instruction;

    if(mpz_cmp_ui(body, 0) == 0) // If body=0, Halt
    {
        instruction.type = Halt;
        HaltInstruction halt = { };
        instruction.instruction.halt = halt;
    }
    else
    {
        mpz_t_pair yz_pair;
        un_pair(&yz_pair, body);
        mpz_t y, z;
        mpz_init_set(y, yz_pair.first);
        mpz_init_set(z, yz_pair.second);

        if(mpz_even_p(y)) // If y is even, Inc
        {
            mpz_fdiv_q_2exp(y, y, 1); // y = y >> 1
            
            instruction.type = Inc;
            instruction.instruction.inc.reg = (int)mpz_get_ui(y);
            instruction.instruction.inc.jmp_lbl = (int)mpz_get_ui(z);
        }
        else // If y is odd, Dec
        {
            mpz_fdiv_q_2exp(y, y, 1); // y = y >> 1
            
            mpz_t_pair jk_pair;
            un_pair_minus_one(&jk_pair, z);
            mpz_t j, k;
            mpz_init_set(j, jk_pair.first);
            mpz_init_set(k, jk_pair.second);
            
            instruction.type = Dec;
            instruction.instruction.dec.reg = (int)mpz_get_ui(y);
            instruction.instruction.dec.jmp_lbl_not_zero = (int)mpz_get_ui(j);
            instruction.instruction.dec.jmp_lbl_zero = (int)mpz_get_ui(k);
            
            mpz_clears(j, k, NULL);
        }
        
        mpz_clears(y, z, NULL);
    }
    
    return instruction;
}

int decode_prog(Instruction* prog[], const mpz_t num)
{
    InstructionArray array;
    
    if(array_init(&array, 1) != 0)
        return -1;
    
    mpz_t body;
    mpz_init(body);
    mpz_t rest;
    mpz_init_set(rest, num);
    while(mpz_cmp_ui(rest, 0) != 0) // while rest is not 0
    {
        mpz_t_pair pair;
        mpz_init(pair.first);
        mpz_init(pair.second);
        list_split(&pair, rest);
        mpz_set(body, pair.first);
        mpz_set(rest, pair.second);
        
        if(array_add(&array, decode_instruction(body)) != 0)
            return -1;
    }
    
    *prog = array.array;
    
    mpz_clears(body, rest, NULL);
    
    return array.used;
}

void pack(void)
{
    Instruction* instructionsv;
    int instructionsc = parse_stdin(&instructionsv);

    mpz_t num;
    mpz_init(num);
    encode_prog(num, instructionsc, instructionsv);
    
    mpz_out_str(stdout, 10, num);
    printf("\n");
    
    free(instructionsv);
    
    mpz_clear(num);
}

void unpack(void)
{    
    mpz_t num;
    mpz_init(num);
    mpz_inp_str(num, stdin, 0);
    
    Instruction* instructionsv;
    int instructionsc = decode_prog(&instructionsv, num);
    
    for(int i = 0; i < instructionsc; i++)
    {
        instruction_to_string(instructionsv[i]);
    }
    
    free(instructionsv);
    mpz_clear(num);
}

int main(int argc, char *argv[])
{
    int c;
    while((c = getopt(argc, argv, "pu")) != -1)
    {
        switch(c)
        {
            case 'p':
                pack();
                break;
            case 'u':
                unpack();
                break;
            case '?':
                return 1;
            default:
                fprintf(stderr, "Usage: packer -[pu]");
                abort();
        }
    }
    
    return 0;
}
