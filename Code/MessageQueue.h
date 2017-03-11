#ifndef MQ_H
#define MQ_H

mqd_t CreateMasterQueue(char* name);
char* Listen(mqd_t mq);
void CloseQueue(mqd_t mq, char* Qname);


#endif
//MQ_H

