#ifndef _OBJ_SERVER_H_
#define _OBJ_SERVER_H_

#include <sys/types.h>
#include <pthread.h>

#include <iostream>
#include <string>
#include <list>

#include "Obj_Msg.h"

using namespace std;

class obj_server {
public:
    pthread_mutex_t o_lock;     // o_lock for object_lock,避免多个线程交叉读写recv_buf和send_buf；
    int64_t server_id;
    int32_t server_type;
    int32_t sock_id;
    string recv_buf;
    string send_buf;
    obj_msg *pO_msg;
public:
    obj_server();
    ~obj_server();
};

#endif
