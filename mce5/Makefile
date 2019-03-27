all: eventQueue.o main.o processList.o
	gcc -g -o dec eventQueue.o main.o processList.o

main.o: main.c main.h
	gcc -g -o main.o -c main.c 
	
eventQueue.o: eventQueue.c eventQueue.h
	gcc -g -o eventQueue.o -c eventQueue.c 
	
processList.o: processList.c processList.h
	gcc -g -o processList.o -c processList.c
	
clean:
	rm -rf *.o 	
