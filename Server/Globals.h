#ifndef _GLOBAL_VAR_H_
#define _GLOBAL_VAR_H_
#include <hiredis/hiredis.h>
#include <lua.hpp>
#include <yaml-cpp/yaml.h>

#include <list>
#include <vector>
#include <map>

#include "Net_Utils.h"
#include "Obj_Server.h"

using namespace std;

extern YAML::Node config;
extern char LISTEN_ADDR[256];
extern int32_t LISTEN_PORT;
extern char HOSTNAME[256];
extern string cut_line;

extern list<obj_server *> pServer_list;

extern void InitGlobal();
extern obj_server *ResolveTheServer(int64_t server_id);
extern obj_server *ResolveTheServerBySock(int32_t sock_id);
extern void WriteMsgToSocket(int32_t conn_sock, string *msg);

#endif
