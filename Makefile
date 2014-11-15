CC = clang
CFLAGS = --std=c++11 -lstdc++ -march=native -O3 -Wall -Wno-comment
DEBUG = -g -D_PRINT_VALS #-g
OBJS = Test.o BigIntCore.o BigIntModular.o

%.o : %.cpp; $(CC) -c -o $@ $< $(CFLAGS) $(DEBUG)

all : $(OBJS) ; $(CC) -o Test $^ $(CFLAGS) $(DEBUG)

test : all ; ./Test


.PHONY: clean

clean : ; rm *.o
