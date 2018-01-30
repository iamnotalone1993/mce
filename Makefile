src = example.c
program = example

.PHONY : run clean

comp : $(src)
	mpicc $(src) -o $(program)

run : $(program)
	mpirun -np 3 --oversubscribe $(program)

clean:
	rm $(program)
