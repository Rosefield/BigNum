CC = clang
CFLAGS = --std=c++11 -lstdc++ -march=native -g -D_PRINT_VALS
OBJS = Test.o

%.o : %.cpp; $(CC) -c -o $@ $< $(CFLAGS)

all : $(OBJS) ; $(CC) -o Test $^ $(CFLAGS)

test : all ; ./Test


.PHONY: clean

clean : ; rm *.o
