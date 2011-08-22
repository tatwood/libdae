EXE=bin/daegen
OBJS=obj/main.o obj/gen.o obj/genc.o obj/genh.o
INCLUDES=
LIBS=
CC=gcc
CCFLAGS=-Wall -O0 -ggdb2 -fno-exceptions -DDEBUG $(INCLUDES)
LD=gcc
LDFLAGS=$(LIBS)

vpath %.c src

$(EXE): obj bin $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXE)

obj:
	mkdir obj

bin:
	mkdir bin

obj/%.o : %.c
	$(CC) $(CCFLAGS) -c $< -o $@

all: $(EXE)

clean:
	rm -rf bin/daegen obj
