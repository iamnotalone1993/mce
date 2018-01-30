src = example.c
program = example
lib = pmpi.c

$(program) : $(src)
	mpicc $(src) -o $(program)

.PHONY : run clean
run : $(program)
	mpirun -np 3 --oversubscribe $(program)

debug : $(lib)
	mpicc -Wall -I./inc -g -c $(lib)

clean:
	rm $(program)
