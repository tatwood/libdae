LIB=lib/libdae.a
LIBD=lib/libdaed.a
OBJS=obj/make.o
OBJSD=objd/make.o
INCLUDES=-Iinclude
CC=gcc
CCFLAGS=-Wall -O3 -fno-exceptions -DNDEBUG $(INCLUDES)
CCFLAGSD=-Wall -O0 -ggdb2 -fno-exceptions -DDEBUG $(INCLUDES)
AR=ar
ARFLAGS=rs

$(LIB): obj lib $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(LIBD): obj lib $(OBJSD)
	$(AR) $(ARFLAGS) $@ $(OBJSD)

obj:
	mkdir obj

objd:
	mkdir objd

lib:
	mkdir lib

obj/make.o : make.c
	$(CC) $(CCFLAGS) -c $< -o $@

objd/make.o : make.c
	$(CC) $(CCFLAGSD) -c $< -o $@

all: $(LIB) $(LIBD)

clean:
	rm -rf lib/libdae.a lib/libdaed.a obj

