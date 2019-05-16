all: eventQueue.o main.o processList.o indexProcessing.o detectQueue.o
	gcc -g -o dec eventQueue.o main.o processList.o indexProcessing.o detectQueue.o -lgmp

main.o: main.c main.h const.h debug.h
	gcc -g -o main.o -c main.c 

eventQueue.o: eventQueue.c eventQueue.h const.h debug.h
	gcc -g -o eventQueue.o -c eventQueue.c 

processList.o: processList.c processList.h const.h debug.h
	gcc -g -o processList.o -c processList.c

indexProcessing.o: indexProcessing.h indexProcessing.c const.h debug.h
	gcc -g -o indexProcessing.o -c indexProcessing.c

detectQueue.o: detectQueue.c detectQueue.h const.h debug.h
	gcc -g -o detectQueue.o -c detectQueue.c

clean:
	rm -rf *.o 	
