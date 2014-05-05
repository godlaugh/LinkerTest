#ifndef _OBJ_MSG_H_
#define _OBJ_MSG_H_

#include <malloc.h>
#include <sys/types.h>

#include <iostream>
#include <string>

#define RECV_BUF_SIZE 1024 * 4

using namespace std;

class recv_buffer {
    public:
        void *begin_at;
        int32_t offset_head;
        int32_t offset_tail;
    public:
        recv_buffer();
        ~recv_buffer();
};

class obj_msg {
    public:
        int32_t recv_length;
        int32_t type;
    //    string reflect_path;
        string content;
    public:
        // obj_msg();
        // ~obj_msg();
};

#endif
