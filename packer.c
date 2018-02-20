#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "instructions.h"
#include "instruction_array.h"
#include "parser.h"
#include "bn.h"
#include "packer.h"

typedef struct
{
    BN first;
    BN second;
} BNPair;

void pair(BN* x, BN* y, BN* out)
{
    BN two;
    bignum_from_int(&two, 2);
    
    BN a, b;
    
    bignum_pow(&two, x, &a); // a = 2^x

    bignum_lshift(y, &b, 1); // b = y * 2
    bignum_inc(&b); // y++
    
    bignum_mul(&a, &b, out); // out = a * b
}

void un_pair(BN* pair, BNPair* out)
{
    BN bitmask;
    bignum_from_int(&bitmask, 1);
    
    //BN x;
    //bignum_from_int(&x, 0);
    int x = 0;
    
    BN comp;
    bignum_and(pair, &bitmask, &comp);
    while(bignum_is_zero(&comp))
    {
        x++;
        //bignum_inc(&x); // x++
        BN t;
        bignum_assign(&t, &bitmask);
        bignum_lshift(&t, &bitmask, 1); // bitmask = bitmask << 1
        bignum_and(pair, &bitmask, &comp);
    }
    
    BN t;
    bignum_assign(&t, &bitmask);
    bignum_lshift(&t, &bitmask, 1); // bitmask = bitmask << 1
    bignum_dec(&bitmask); // bitmask = bitmask - 1
    bignum_not(&bitmask, &bitmask); // bitmask = ~bitmask
    
    BN y;
    bignum_and(pair, &bitmask, &y);
    bignum_rshift(&y, &y, x+1); // y = (pair & bitmask) >> (x+1)
    
    BN bnx;
    bignum_from_int(&bnx, x);
    
    out->first = bnx;
    out->second = y;
}

void pair_minus_one(BN* x, BN* y, BN* out)
{
    pair(x, y, out);
    bignum_dec(out);
}

void un_pair_minus_one(BN* pair, BNPair* out)
{
    bignum_inc(pair); // pair++
    un_pair(pair, out);
}
  

void list_cons(BN* head, BN* tail, BN* out)
{
    pair(head, tail, out);
}

void list_split(BN* list, BNPair* out)
{
    un_pair(list, out);
}

void prog_to_num(int instructionsc, Instruction* instructionsv, BN* out)
{
    bignum_from_int(out, 0);
    
    for(int i = instructionsc - 1; i >= 0; i--)
    {
        Instruction instruction = instructionsv[i];
        BN body;
        bignum_from_int(&body, 0);
        
        if(instruction.type == Inc)
        {
            IncrementInstruction inc = instruction.instruction.inc;
            
            BN x;
            bignum_from_int(&x, 2 * inc.reg);
            
            BN y;
            bignum_from_int(&y, inc.jmp_lbl);
            
            pair(&x, &y, &body);
        }
        else if(instruction.type == Dec)
        {
            DecrementInstruction dec = instruction.instruction.dec;
            
            BN x;
            bignum_from_int(&x, 2 * dec.reg + 1);
            
            BN y;
            bignum_from_int(&y, dec.jmp_lbl_not_zero);
            
            BN z;
            bignum_from_int(&z, dec.jmp_lbl_zero);
            
            BN p;
            pair_minus_one(&y, &z, &p);
            pair(&x, &p, &body);
        }
        else if(instruction.type == Halt)
        {
            bignum_from_int(&body, 0);
        }
        else
        {
            //Unrecognised
        }
        
        list_cons(&body, out, out);
    }
}

Instruction decode_body(BN* body)
{
    Instruction instruction;

    if(bignum_is_zero(body)) //Halt
    {
        instruction.type = Halt;
        HaltInstruction halt = { };
        instruction.instruction.halt = halt;
    }
    else
    {
        BNPair yz_pair;
        un_pair(body, &yz_pair);
        BN y = yz_pair.first;
        BN z = yz_pair.second;

        if((y.array[0] & 1) == 0) //Even, INC
        {
            BN i;
            bignum_rshift(&y, &i, 1); // i = y >> 1
            
            instruction.type = Inc;
            instruction.instruction.inc.reg = bignum_to_int(&i);
            instruction.instruction.inc.jmp_lbl = bignum_to_int(&z);
        }
        else //Odd, DEC
        {
            BN i;
            bignum_rshift(&y, &i, 1); // i = y >> 1
            
            BNPair jk_pair;
            un_pair_minus_one(&z, &jk_pair);
            BN j = jk_pair.first;
            BN k = jk_pair.second;
            
            instruction.type = Dec;
            instruction.instruction.dec.reg = bignum_to_int(&i);
            instruction.instruction.dec.jmp_lbl_not_zero = bignum_to_int(&j);
            instruction.instruction.dec.jmp_lbl_zero = bignum_to_int(&k);
        }
    }
    
    return instruction;
}

int num_to_prog(Instruction* instructions[], BN* num)
{
    InstructionArray array;
    
    if(array_init(&array, 1) != 0)
        return -1;
    
    BN body;
    BN rest = *num;
    while(!bignum_is_zero(&rest))
    {
        BNPair pair;
        list_split(&rest, &pair);
        body = pair.first;
        rest = pair.second;
        
        if(array_add(&array, decode_body(&body)) != 0)
            return -1;
    }
    
    *instructions = array.array;
    
    return array.used;
}

void pack(void)
{
    Instruction* instructionsv;
    int instructionsc = parse_stdin(&instructionsv);

    BN num;
    prog_to_num(instructionsc, instructionsv, &num);
    
    char buf[8192];
    bignum_to_string(&num, buf, sizeof(buf));
    int i = bignum_to_int(&num);
    printf("%s\n", buf);
    
    free(instructionsv);
}

void print_instruction_string(Instruction instruction)
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

void unpack(void)
{
    char buf[1024];
    scanf("%s", &buf);
    
    BN num;
    bignum_from_string(&num, buf, WORD_SIZE * 2); //TODO: implement this myself, their implementation is shit
    
    Instruction* instructionsv;
    int instructionsc = num_to_prog(&instructionsv, &num);
    
    for(int i = 0; i < instructionsc; i++)
    {
        print_instruction_string(instructionsv[i]);
    }
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
