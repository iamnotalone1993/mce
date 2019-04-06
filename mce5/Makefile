all: eventQueue.o main.o processList.o indexProcessing.o
	gcc -g -o dec eventQueue.o main.o processList.o indexProcessing.o

main.o: main.c main.h
	gcc -g -o main.o -c main.c 
	
eventQueue.o: eventQueue.c eventQueue.h
	gcc -g -o eventQueue.o -c eventQueue.c 
	
processList.o: processList.c processList.h
	gcc -g -o processList.o -c processList.c
	
indexProcessing.o: indexProcessing.h indexProcessing.c
	gcc -g -o indexProcessing.o -c indexProcessing.c

clean:
	rm -rf *.o 	
