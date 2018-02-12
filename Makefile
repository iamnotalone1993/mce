CC := mpicc
ana := analyzer
bench_dir := ./benchmarks/
bench := example
prog := $(bench_dir)$(bench)
preProg := out
lib := profiler
dec := detector
num_procs := 3

$(lib).o : $(lib).c

$(dec) : $(dec).c

lib.a : $(lib).o
	ar -cr $@ $^

.PHONY : ana all run dec clean
$(ana) : $(ana).c

#type "make ana" to analyze
ana : $(ana)
	./$(ana) $(prog).c $(preProg).c

#type "make all" to compile
all : $(preProg).c lib.a
	$(CC) -Wall $^ -o $(preProg)

#type "make run" to run
run : $(preProg)
	mpirun -np $(num_procs) --oversubscribe $(preProg)

#type "make dec" to detect
dec : $(dec)
	./$(dec) $(num_procs)

#type "make clean" to clean
clean:
	rm -f lib.a $(ana) $(preProg)* $(prog) $(lib).o trace* $(dec)
