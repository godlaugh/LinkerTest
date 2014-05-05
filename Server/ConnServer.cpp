#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <exception>

#include "Net_Utils.h"
#include "Utils.h"
#include "Obj_Server.h"
#include "Globals.h"

#define MAX_PROCS 10

using namespace std;

static uint64_t recv_counter;

void handle_first_connected(EPOLL_VARS *epoll_vars) {
    obj_server *pO_server = new obj_server();
    pO_server->sock_id = epoll_vars->ev.data.fd;
    pServer_list.push_back(pO_server);
    cout << "Have a server connected!" << endl;
}

void handle_disconnect(int32_t conn_sock) {
    obj_server *pO_server = ResolveTheServerBySock(conn_sock);
    for (list<obj_server *>::iterator pL_server_it = pServer_list.begin(); pL_server_it != pServer_list.end(); ++pL_server_it) {
        // 最后删除该连接过来的服务器；
        if (pO_server->sock_id == (*pL_server_it)->sock_id) {
            delete *pL_server_it;
            cout << "Have a server disconnected!" << endl;
            pL_server_it = pServer_list.erase(pL_server_it);
            break;
        }
    }
}

void handle_received_bit_stream(int32_t conn_sock) {
    recv_counter++;
    // 根据socket解析出连接过来的服务器；
    obj_server *pO_server = ResolveTheServerBySock(conn_sock);
    // 追加接收的数据到服务器对象的recv_buf中；
    pO_server->recv_buf.append(NetBuffer.buffer, NetBuffer.n);
    // cout << conn_sock << ": " << pO_server->recv_buf;
    WriteMsgToSocket(pO_server->sock_id, &pO_server->recv_buf);
    cout << "SN: " << recv_counter << " : " << pO_server->recv_buf;
    pO_server->recv_buf.clear();
}

void handle_after_first_connected(EPOLL_VARS *epoll_vars, int32_t which) {
    int32_t n, epollfd = epoll_vars->epollfd, conn_sock = epoll_vars->events[which].data.fd;
    while(1) {
        bzero((char *) &NetBuffer.buffer, NET_BUF_SIZE);
        NetBuffer.n = read(conn_sock, NetBuffer.buffer, NET_BUF_SIZE);
        if (0 == NetBuffer.n) {
            if (-1 != epoll_ctl(epollfd, EPOLL_CTL_DEL, conn_sock, NULL)) {
                handle_disconnect(conn_sock);
            } else {
                error("epoll_ctl: delete conn_sock!");
                exit(EXIT_FAILURE);
            }
            close(conn_sock);
            return;
        }
        if (0 > NetBuffer.n) {
            if (EAGAIN == errno) {
            }
            return;
        }
        if (0 != NetBuffer.n) {
            handle_received_bit_stream(conn_sock);
        }
    }
}

int32_t main(int32_t argc, const char *argv[]) {
    recv_counter = 0;
    catch_crash();
    cout << "LISTEN_ADDR: " << LISTEN_ADDR << endl;
    memcpy(LISTEN_ADDR, "0.0.0.0", sizeof("0.0.0.0"));
    LISTEN_SOCK listen_sock = init_listen_sock(7887, LISTEN_ADDR);
    EPOLL_VARS epoll_vars = init_epoll(listen_sock.listen_sock);
    handle_sock_from_epoll(&listen_sock, &epoll_vars, handle_first_connected, handle_after_first_connected);
    return 0;
}
