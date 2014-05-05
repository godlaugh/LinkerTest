#include "Net_Utils.h"

NET_BUF NetBuffer;

LISTEN_SOCK init_listen_sock(int32_t portno, char listen_addr[]) {
    // int32_t listen_sock;
    // socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    LISTEN_SOCK l_s;

    l_s.listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    int32_t on = 1;
    setsockopt(l_s.listen_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (0 > l_s.listen_sock) {
        error("ERROR: opening socket!");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(inet_addr(listen_addr));
    serv_addr.sin_port = htons(portno);
    if (0 > bind(l_s.listen_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) {
        error("ERROR: on binding!");
    }
    listen(l_s.listen_sock,5);
    l_s.clilen = sizeof(cli_addr);
    l_s.cli_addr = cli_addr;
    return l_s;
}

EPOLL_VARS init_epoll(int32_t listen_sock) {
    // int32_t epollfd, n;
    // struct epoll_event ev, events[MAX_EVENTS];
    EPOLL_VARS e_vs;

    e_vs.epollfd = epoll_create(10);
    if (-1 == e_vs.epollfd) {
        error("ERROR: By epoll_create!");
        exit(EXIT_FAILURE);
    }
    e_vs.ev.events = EPOLLIN;
    e_vs.ev.data.fd = listen_sock;
    if (-1 == epoll_ctl(e_vs.epollfd, EPOLL_CTL_ADD, listen_sock, &e_vs.ev)) {
        error("ERROR: By epoll_ctl listen_sock!");
        exit(EXIT_FAILURE);
    }
    return e_vs;
}

void handle_sock_from_epoll(LISTEN_SOCK *listen_sock, EPOLL_VARS *epoll_vars, void (*first_conn)(EPOLL_VARS *epoll_vars), void (*after_first_conn)(EPOLL_VARS *epoll_vars, int32_t which)) {
    int32_t conn_sock, nfds;  /* nfds for notify file descriptors */
    while(true) {
        nfds = epoll_wait(epoll_vars->epollfd, epoll_vars->events, MAX_EVENTS, -1);
        if (-1 == nfds) {
            printf("%s:%d\n",strerror(errno),errno);
            error("ERROR: By epoll_pwait!");
            exit(EXIT_FAILURE);
        }
        for (int32_t i = 0; i < nfds; ++i) {
            if (epoll_vars->events[i].data.fd == listen_sock->listen_sock) {
                conn_sock = accept(listen_sock->listen_sock, (struct sockaddr *) &listen_sock->cli_addr, &listen_sock->clilen);
                if (-1 == conn_sock) {
                    error("ERROR: By accept!");
                    exit(EXIT_FAILURE);
                }
                setnonblocking(conn_sock);
                epoll_vars->ev.events = EPOLLIN | EPOLLET;
                epoll_vars->ev.data.fd = conn_sock;
                if (-1 != epoll_ctl(epoll_vars->epollfd, EPOLL_CTL_ADD, conn_sock, &epoll_vars->ev)) {
                    first_conn(epoll_vars);
                } else {
                    error("ERROR: By epoll_ctl conn_sock!");
                    exit(EXIT_FAILURE);
                }
            } else {
                after_first_conn(epoll_vars, i);
            }
        }
    }
    close(listen_sock->listen_sock);
}
