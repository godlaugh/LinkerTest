#include "Obj_Msg.h"

using namespace std;

recv_buffer::recv_buffer() {
    this->offset_head = 0;
    this->offset_tail = 0;
    this->begin_at = malloc(RECV_BUF_SIZE);
}

recv_buffer::~recv_buffer() {
    free(this->begin_at);
}
