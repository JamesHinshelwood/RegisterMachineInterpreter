HEADERS_I = instruction_array.h instructions.h parser.h
OBJECTS_I = instruction_array.o interpreter.o parser.o
HEADERS_P = packer.h instruction_array.h instructions.h parser.h
OBJECTS_P = packer.o instruction_array.o parser.o
CFLAGS = -Wall -g -lgmp

default: all

all: interpreter packer

%.o: %.c $(HEADERS_I)
	gcc -c $< -o $@ $(CFLAGS)

interpreter: $(OBJECTS_I)
	gcc $(OBJECTS_I) -o $@ $(CFLAGS)

%.o: %.c $(HEADERS_P)
	gcc -c $< -o $@ $(CFLAGS)

packer: $(OBJECTS_P)
	gcc $(OBJECTS_P) -o $@ $(CFLAGS)

clean:
	-rm -f $(OBJECTS_I)
	-rm -f $(OBJECTS_P)
	-rm -f interpreter
	-rm -f packer
