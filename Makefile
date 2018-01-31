src = example.c
program = example
lib = pmpi.c
obj = pmpi.o

$(obj) : $(lib)
	mpicc -c $^

lib.a : $(obj)
	ar -cr $@ $^

.PHONY : run debug clean
#type "make all" to compile
all : $(src) lib.a
	mpicc -Wall $^ -o $(program)

#type "make run" to run
run : $(program)
	mpirun -np 3 --oversubscribe $(program)

#type "make clean" to clean
clean:
	rm -f $(obj) lib.a $(program) trace*
