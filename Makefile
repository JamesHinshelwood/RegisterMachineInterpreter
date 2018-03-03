CFLAGS = -Wall

vpath %.c src
vpath %.h src

default: all

debug: CFLAGS += -g
debug: all

all: interpreter packer

interpreter: parser.o instructions.o instruction_array.o

packer: LDLIBS = -lgmp parser.o instructions.o instruction_array.o

clean:
	$(RM) *.o interpreter packer