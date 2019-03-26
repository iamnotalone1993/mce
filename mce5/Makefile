all: eventQueue.o main.o processList.o
	gcc -o dec eventQueue.o main.o processList.o

main.o: main.c main.h
	gcc -o main.o -c main.c 
	
eventQueue: eventQueue.c eventQueue.h
	gcc -o eventQueue.o -c eventQueue.c 
	
processList.o: processList.c processList.h
	gcc -o processList.o -c processList.c
	
clean:
	rm -rf *.o 	
