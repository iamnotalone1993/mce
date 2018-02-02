CC := mpicc
ana := analyzer
bench_dir := ./benchmarks/
bench := example
prog := $(bench_dir)$(bench)
preProg := out
lib := pmpi

$(lib).o : $(lib).c

lib.a : $(lib).o
	ar -cr $@ $^

.PHONY : pre ana all run clean
$(ana) : $(ana).c

#type "make ana" to analyze
ana : $(ana)
	./$(ana) $(prog).c $(preProg).c

#type "make all" to compile
all : $(preProg).c lib.a
	$(CC) -Wall $^ -o $(preProg)

#type "make run" to run
run : $(preProg)
	mpirun -np 3 --oversubscribe $(preProg)

#type "make clean" to clean
clean:
	rm -f lib.a $(ana) $(preProg)* $(prog) $(lib).o trace*
