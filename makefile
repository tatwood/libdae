LIB=lib/libdae.a
LIBD=lib/libdaed.a
OBJS=obj/dae.o obj/daeu.o
OBJSD=obj/dae.d.o obj/daeu.d.o
INCLUDES=-Iinclude
CC=gcc
CCFLAGS=-Wall -O2 -fno-exceptions -DNDEBUG $(INCLUDES)
CCFLAGSD=-Wall -O0 -ggdb2 -fno-exceptions -DDEBUG $(INCLUDES)
AR=ar
ARFLAGS=rs

vpath %.c src

$(LIB): obj lib $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(LIBD): obj lib $(OBJSD)
	$(AR) $(ARFLAGS) $@ $(OBJSD)

obj:
	mkdir obj

lib:
	mkdir lib

obj/%.o : %.c
	$(CC) $(CCFLAGS) -c $< -o $@

obj/%.d.o : %.c
	$(CC) $(CCFLAGSD) -c $< -o $@

all: $(LIB) $(LIBD)

clean:
	rm -f lib/libdae.a lib/libdaed.a obj/*
