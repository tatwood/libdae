EXE=bin/daegen
EXED=bin/daegend
OBJS=obj/make.o
OBJSD=objd/make.o
INCLUDES= -I../../extern/expat/lib
LIBS= -L../../extern/expat/.libs -lexpat
CC=gcc
CCFLAGS=-Wall -msse -O3 -fno-exceptions -DNDEBUG -DXML_STATIC $(INCLUDES)
CCFLAGSD=-Wall -msse -O0 -ggdb2 -fno-exceptions -D_DEBUG -DXML_STATIC $(INCLUDES)
LD=gcc
LDFLAGS=$(LIBS)

$(EXE): obj bin $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXE)

$(EXED): objd bin $(OBJSD)
	$(LD) $(OBJSD) $(LDFLAGS) -o $(EXED)

obj:
	mkdir obj

objd:
	mkdir objd

bin:
	mkdir bin

obj/make.o : make.c
	$(CC) $(CCFLAGS) -c $< -o $@

objd/make.o : make.c
	$(CC) $(CCFLAGSD) -c $< -o $@

all: $(EXE) $(EXED)

clean:
	rm -rf $(EXE) $(EXED) obj objd

debug: $(EXED)

release: $(EXE)
