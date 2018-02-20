# Register Machine Interpreter and Packer
Register machine interpreter written in C
Register machines and packed integer form are implemented as described [here](http://www.cl.cam.ac.uk/teaching/1718/CompTheory/CompTheory.pdf).

### Compiling

Can be compiled with `make`. You might have to change the compiler in `Makefile`.

### Running

#### Interpreter

`interpreter` reads a program from stdin, and takes initial register values as arguments.

For example, `cat add.r | ./interpreter 0 7 8`, will run a register machine that computes R0=R1+R2=7+8.

#### Packer

`packer [-p|-u]` packs or unpacks a program from stdin and outputs the result. `-p` converts from a normal program to a packed integer. `-u` converts from a packed integer into a program.

For example, `cat add.r | ./packer -p` will convert the add program into an integer representation of it.  
`cat add.r | ./packer -p | ./packer -u | ./interpreter 0 81 129` will convert add to an integer and back again, then run the program.

### Register Machine Programs

The syntax of register programs is from [here](https://github.com/SophieDurrant/RegisterMachineSimulator). Thank you 🙂

Each line of a file must be in one of the following formats:

1. `HALT` will stop the program
2. `INC REGISTER NEXT_LABEL` will increment `REGISTER` and jump to `NEXT_LABEL`
3. `DEC REGISTER LABEL_1 LABEL_2` will decrement `REGISTER` and jump to `LABEL_1` if `REGISTER` is greater than zero, otherwise it will jump to `LABEL_2`

`REGISTER`, `NEXT_LABEL`, `LABEL_1`, and `LABEL_2` must be nonnegative integers.

The first line of the file is L0 in the register machine, the second line is L1, and so on.
