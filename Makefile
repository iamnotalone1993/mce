ana = analyzer
prog = example
preProg = out
lib = pmpi

$(lib).o : $(lib).c
	mpicc -c $^

lib.a : $(lib).o
	ar -cr $@ $^

.PHONY : pre ana all run clean
$(ana) : $(ana).c

ana : $(ana)
	./$(ana) $(prog).c $(preProg).c

#type "make all" to compile
all : $(preProg).c lib.a
	mpicc -Wall $^ -o $(preProg)

#type "make run" to run
run : $(preProg)
	mpirun -np 3 --oversubscribe $(preProg)

#type "make clean" to clean
clean:
	rm -f lib.a $(ana) $(preProg)* $(prog) $(lib).o trace*
