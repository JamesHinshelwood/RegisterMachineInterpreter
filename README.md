# Register Machine Interpreter
Register machine interpreter written in C

#### Compiling

Can be compiled with `make`. You might have to change the compiler in `Makefile`.

#### Running

`interpreter` reads a program from stdin, and takes initial register values as arguments.

For example, `cat add.r | ./interpreter 0 7 8`, will run a register machine that computes R0=R1+R2=7+8.

#### Register Machine Programs

The syntax of register programs is from [here](https://github.com/SophieDurrant/RegisterMachineSimulator). Thank you 🙂

Each line of a file must be in one of the following formats:

1. `HALT` will stop the program
2. `INC REGISTER NEXT_LABEL` will increment `REGISTER` and jump to `NEXT_LABEL`
3. `DEC REGISTER LABEL_1 LABEL_2` will decrement `REGISTER` and jump to `LABEL_1` if `REGISTER` is greater than zero, otherwise it will jump to `LABEL_2`

`REGISTER`, `NEXT_LABEL`, `LABEL_1`, and `LABEL_2` must be nonnegative integers.

The first line of the file is L0 in the register machine, the second line is L1, and so on.
