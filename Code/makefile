objects = main.o logger.o MessageQueue.o timer.o
headers = log.h MessageQueue.h timer.h

DaemonTest : $(objects)
	gcc -o DaemonTest $(objects) -lrt

main.o : main.c $(headers)
	gcc -c main.c -lrt

timer.o : timer.c timer.h log.h
	gcc -c timer.c -lrt 

logger.o : logger.c log.h 
	gcc -c logger.c 

MessageQueue.o : MessageQueue.c MessageQueue.h 
	gcc -c MessageQueue.c -lrt 

clean: 
	rm $(objects) DaemonTest