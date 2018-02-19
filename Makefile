HEADERS = instruction_array.h instructions.h parser.h
OBJECTS = instruction_array.o interpreter.o parser.o
CFLAGS = -Wall

default: interpreter

%.o: %.c $(HEADERS)
	gcc -c $< -o $@ $(CFLAGS)

interpreter: $(OBJECTS)
	gcc $(OBJECTS) -o $@ $(CFLAGS)

clean:
	-rm -f $(OBJECTS)
	-rm -f interpreter
