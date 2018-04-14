CC := mpicc
bench_dir := ./benchmarks/
bench := example
prog := $(bench_dir)$(bench)
lib := profiler
dec := detector
num_procs := 3

$(lib).o : $(lib).c

$(dec) : $(dec).h

lib.a : $(lib).o
	ar -cr $@ $^

.PHONY : all run dec clean

#type "make all" to compile
all : $(prog).c lib.a
	$(CC) -Wall $^ -o $(prog)

#type "make run" to run
run : $(prog)
	mpirun -np $(num_procs) --oversubscribe $(prog)

#type "make dec" to detect
dec : $(dec)
	./$(dec) $(num_procs)

#type "make clean" to clean
clean:
	rm -f lib.a $(prog) $(lib).o ./traces/* $(dec)
