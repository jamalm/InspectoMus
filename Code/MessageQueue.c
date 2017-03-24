#include<stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "MessageQueue.h"
#include "log.h"


//this is mostly for clean readable code in the daemon program
//creates the initial queue
mqd_t CreateMasterQueue(char* name)
{

    mqd_t mq;
    struct mq_attr q_attrs;

    char buffer [1024+1];
    int terminate = 0;

    /* Set Attributes of Queue*/

    q_attrs.mq_flags = 0;
    q_attrs.mq_maxmsg = 10;
    q_attrs.mq_msgsize = 1024;
    q_attrs.mq_curmsgs = 0;

    /* create Queue */
    mq = mq_open("/Daemon_Manager", O_CREAT | O_RDONLY, 0755, &q_attrs);

    if(mq == -1)
    {
        LogErrDaemon("Queue", "Error Occured");
    }
    return mq;
}

//allows the daemon to listen for a message
char* Listen(mqd_t mq)
{
    //char buffer [1024+1];
    char *buffer = (char *)malloc(sizeof(char)*(1024+1));
    while(1)
    {
        ssize_t bytes_read;

        /*receive message*/
        bytes_read = mq_receive(mq, buffer, 1024, NULL);

        buffer[bytes_read] = '\0';
        return buffer;
    }
}


//safely cleans up queue at termination of daemon
void CloseQueue(mqd_t mq, char* Qname)
{

    LogDaemon("Queue", "Closed");
    mq_close(mq);
    mq_unlink(Qname);

}
