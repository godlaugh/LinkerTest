#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <list>
#include <vector>
#include <map>

#include "Globals.h"
#include "Utils.h"
#include "Obj_Server.h"

using namespace std;
char LISTEN_ADDR[256];
int32_t LISTEN_PORT;

char HOSTNAME[256];
string cut_line = "+++++++++++++++++++++++++++++++++++++++++++++++++++++++";

list<int32_t>::iterator l_i32_it;
vector<int32_t>::iterator v_i32_it;

list<obj_server *> pServer_list;

void InitGlobal() {
    gethostname(HOSTNAME, sizeof(HOSTNAME));
}

obj_server *ResolveTheServer(int64_t server_id) {
    for (list<obj_server *>::iterator pL_server_it = pServer_list.begin(); pL_server_it != pServer_list.end(); ++pL_server_it) {
        if (server_id == (*pL_server_it)->server_id) {
            return &**pL_server_it;
            break;
        }
    }
    return NULL;
}

obj_server *ResolveTheServerBySock(int32_t sock_id) {
    for (list<obj_server *>::iterator pL_server_it = pServer_list.begin(); pL_server_it != pServer_list.end(); ++pL_server_it) {
        if (sock_id == (*pL_server_it)->sock_id) {
            return &**pL_server_it;
            break;
        }
    }
    return NULL;
}

void WriteMsgToSocket(int32_t conn_sock, string *pMsg) {
    int32_t wn = write(conn_sock, pMsg->data(), pMsg->size());
    if (wn < 0) {
        error("ERROR writing to socket");
    }
}

