daemonObjects = main.o logger.o MessageQueue.o timer.o Auditor.o Backup.o
programs = InspectoMus
headers = log.h MessageQueue.h timer.h Auditor.h Backup.h


DaemonTest : $(daemonObjects)
	gcc -o InspectoMus $(daemonObjects) -lrt -g


main.o : main.c $(headers)
	gcc -c main.c -lrt -g
	
Backup.o : Backup.c $(headers)
	gcc -c Backup.c -lrt -g
	
Auditor.o : Auditor.c $(headers)
	gcc -c Auditor.c -lrt -g

timer.o : timer.c timer.h log.h
	gcc -c timer.c -lrt -g

logger.o : logger.c log.h 
	gcc -c logger.c -g

MessageQueue.o : MessageQueue.c MessageQueue.h 
	gcc -c MessageQueue.c -lrt -g

clean: 
	rm $(daemonObjects) $(programs)
