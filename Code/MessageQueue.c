#include<stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "MessageQueue.h"



mqd_t CreateMasterQueue(char* name)
{

    mqd_t mq;
    struct mq_attr q_attrs;

    char buffer [1024+1];
    int terminate = 0;

    /* Set Attributes of Queue*/

    q_attrs.mq_flags = 0;
    q_attrs.mq_maxmsg = 100;
    q_attrs.mq_msgsize = 1024;
    q_attrs.mq_curmsgs = 0;

    /* create Queue */
    mq = mq_open(name, O_CREAT | O_RDONLY, 0644, &q_attrs);
    if(mq == -1)
    {
        printf("\nerror encountered %s\n", strerror(errno));
    }
    return mq;
}

char* Listen(mqd_t mq)
{
    char buffer [1024+1];
    while(1)
    {
        ssize_t bytes_read;

        /*receive message*/
        bytes_read = mq_receive(mq, buffer, 1024, NULL);

        buffer[bytes_read] = '\0';
        if(strncmp(buffer, '\0', sizeof(buffer)) > 0)
        {
            return buffer;
        }
    }
}

void CloseQueue(mqd_t mq, char* Qname)
{
    printf("Closing Queue..\n");
    mq_close(mq);
    mq_unlink(Qname);

}
