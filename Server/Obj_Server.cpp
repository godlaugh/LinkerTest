#include <sys/types.h>
#include <pthread.h>

#include <iostream>
#include <string>
#include <list>

#include "Obj_Server.h"
#include "Obj_Msg.h"

using namespace std;

obj_server::obj_server() {
    this->pO_msg = new obj_msg;
    pthread_mutex_init(&this->o_lock, NULL);
}

obj_server::~obj_server() {
    delete this->pO_msg;
    pthread_mutex_destroy(&this->o_lock);
}
