#ifndef PARSER_H
#define PARSER_H
int parse_line(char* line, Instruction* instruction);

int parse_stdin(Instruction* instructions[]);
#endif
