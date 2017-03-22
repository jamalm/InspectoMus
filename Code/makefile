objects = main.o logger.o MessageQueue.o timer.o
headers = log.h MessageQueue.h timer.h

DaemonTest : $(objects)
	gcc -o DaemonTest $(objects) -lrt -g

main.o : main.c $(headers)
	gcc -c main.c -lrt -g

timer.o : timer.c timer.h log.h
	gcc -c timer.c -lrt -g

logger.o : logger.c log.h 
	gcc -c logger.c -g

MessageQueue.o : MessageQueue.c MessageQueue.h 
	gcc -c MessageQueue.c -lrt -g

clean: 
	rm $(objects) DaemonTest
