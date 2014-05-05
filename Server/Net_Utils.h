#ifndef _NET_UTILS_H_
#define _NET_UTILS_H_
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include "Utils.h"

#define MAX_EVENTS 10
#define NET_BUF_SIZE 1024 * 4

typedef struct {
    int32_t listen_sock;
    struct sockaddr_in cli_addr;
    socklen_t clilen;
} LISTEN_SOCK;

typedef struct {
    int32_t epollfd;
    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];
} EPOLL_VARS;

typedef struct {
    char buffer[NET_BUF_SIZE];
    int32_t n;
} NET_BUF;

extern NET_BUF NetBuffer;
extern LISTEN_SOCK init_listen_sock(int32_t portno, char listen_addr[]);
extern EPOLL_VARS init_epoll(int32_t listen_sock);
extern void handle_sock_from_epoll(LISTEN_SOCK *listen_sock, EPOLL_VARS *epoll_vars, void (*first_conn)(EPOLL_VARS *epoll_vars), void (*after_first_conn)(EPOLL_VARS *epoll_vars, int32_t which));

#endif
