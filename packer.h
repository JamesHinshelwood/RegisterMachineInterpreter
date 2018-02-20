#ifndef PACKER_H
#define PACKER_H
typedef struct bn BN;

void prog_to_num(int instructionsc, Instruction* instructionsv, BN* out);

int num_to_prog(Instruction* instructions[], BN* num);
#endif
