#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <curl/curl.h>
#include <sys/resource.h>
#include <syslog.h>
#include <string.h>
#include <stdarg.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#include "Obj_Msg.h"

#define BUF_SIZE 1024 * 4
#define SUB_STR_SIZE 10 //正则匹配，子字符串的存储区个数
#define LOG_LEVEL LOG_DEBUG

using namespace std;

extern char LOG_DATE[16];
extern char LOG_PATH[];
extern char log_buf[BUFSIZ];
extern FILE *LOG_FP;

template <class T> int32_t getArrayLen(T& array) { return (sizeof(array) / sizeof(array[0])); }
extern void error(const char *pMsg);
extern string num2str(int64_t i);
extern void setnonblocking(int32_t fd);
extern void ParseFromString(obj_msg *pO_msg, string *pStr);

template <class T1>
void ConvAndAppendToString(string *s, const T1 *p) {
    int32_t len = sizeof(*p);
    char tmp[len];
    memcpy(&tmp, p, len);
    s->append(tmp, len);
}

extern void log_time();
extern int logger_init();
extern void logger( int level, char *fmt, ...);
extern char* substr(const char*str, unsigned start, unsigned end);
extern char* dirname(char *full_path);
// extern char* basename(char *full_path);
extern int32_t regex_match(regmatch_t *pm, char *str, char *pattern);
extern size_t WriteFileCallback(void *contents, size_t size, size_t nmemb, void *dst_path);
extern bool get_file_to(char *src_url, char *dst_path);
extern void get_file_in_list_to(char *base_url, char *list_path);
extern string UrlEncode(const string& szToEncode);
extern string UrlDecode(const std::string& szToDecode);
extern string base64_encode(char const* bytes_to_encode, unsigned int in_len);
extern string base64_decode(string const& encoded_string);

extern int32_t catch_crash();

template <class T1, class T2>
void StructuredToString(string *s, const T1 p1, const T2 p2, const char *c) {
    /* *c for content */
    s->clear();
    s->append((const char *) &p1, sizeof(T1));
    s->append((const char *) &p2, sizeof(T2));
    s->append(c, p1);
}

#endif
