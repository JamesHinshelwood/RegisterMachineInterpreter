#ifndef PACKER_H
#define PACKER_H

void prog_to_num(const int instructionsc, const Instruction* instructionsv, mpz_t result);

int num_to_prog(Instruction* instructions[], mpz_t num);
#endif
