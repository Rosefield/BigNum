CC = clang
CFLAGS = --std=c++11 -lstdc++ -march=native -O2 -Wall -Wno-comment
DEBUG = -D_PRINT_VALS -g
OBJS = BigIntCore.o BigIntModular.o

%.o : %.cpp; $(CC) -c -o $@ $< $(CFLAGS) $(DEBUG)

all : $(OBJS) ; $(CC) -o Test Test.cpp $^ $(CFLAGS) $(DEBUG)

test : all ; ./Test

lib : all; ar -qc libBigInt.a $(OBJS)


.PHONY: clean

clean : ; rm *.o
