HEADERS = instruction_array.h instructions.h parser.h
OBJECTS = instruction_array.o interpreter.o parser.o

default: interpreter

%.o: %.c $(HEADERS)
	gcc -c $< -o $@

interpreter: $(OBJECTS)
	gcc $(OBJECTS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f interpreter
