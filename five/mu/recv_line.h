#ifndef RECV_LINE_H__
#define RECV_LINE_H__

#include <sys/socket.h>

ssize_t recv_line(int client_fd,char *buf,size_t size);

#endif