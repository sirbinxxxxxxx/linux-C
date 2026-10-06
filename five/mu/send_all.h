#ifndef SEND_ALL_H__
#define SEND_ALL_H__

#include <sys/socket.h>

int send_all(int client_fd,char * buf,ssize_t size);



#endif