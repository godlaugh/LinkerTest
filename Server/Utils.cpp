#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <regex.h>
#include <fcntl.h>

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#include "Utils.h"
#include "Obj_Msg.h"

using namespace std;

char LOG_DATE[16];
char LOG_PATH[] = "/var/log/my-server.log";
char log_buf[BUFSIZ];
FILE *LOG_FP;

static const string base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

void error(const char *pMsg) { perror(pMsg); exit(1); }
void setnonblocking(int32_t fd) {
    int32_t opts = fcntl(fd, F_GETFL);
    if(opts<0) {
        cout << "fcntl(fd, GETFL)" << endl;
        exit(1);
    }
    opts = opts | O_NONBLOCK;
    if(fcntl(fd, F_SETFL, opts) < 0) {
        cout << "fcntl(fd,SETFL,opts)" << endl;
        exit(1);
    }
}

string num2str(int64_t i) {
    stringstream ss;
    ss << i;
    return ss.str();
}

void ParseFromString(obj_msg *pO_msg, string *pStr) {
    /*
     * |---------------------------obj_msg--------------------------|
     * +------------------+----------------+------------------------+
     * | <int32_t length> | <int32_t type> |    <string content>    |
     * +------------+-----+----------------+------------------------+
     *              |                      ^                        ^
     *              |                      |----------length--------|
     *              |                                    ^
     *              +------------------------------------|
     */
    pO_msg->content.clear();
    int32_t cur_offset = 0;
    memcpy(&pO_msg->recv_length, pStr->data() + cur_offset, sizeof(pO_msg->recv_length));
    cur_offset += sizeof(pO_msg->recv_length);
    memcpy(&pO_msg->type, pStr->data() + cur_offset, sizeof(pO_msg->type));
    cur_offset += sizeof(pO_msg->type);
    pO_msg->content.append(pStr->data() + cur_offset, pO_msg->recv_length);
    pStr->erase(0, pO_msg->recv_length + sizeof(pO_msg->recv_length) + sizeof(pO_msg->type));
}

/* 更新LOG_DATE时间*/
void log_time() {
    time_t timestamp;
    timestamp = time(NULL);
    struct tm *lt = localtime(&timestamp);    // lt for local time;
    strftime(LOG_DATE, sizeof(LOG_DATE), "%b %d %H:%M:%S", lt);
}

/* 日志输出*/
int logger_init() {
    LOG_FP = fopen(LOG_PATH, "a+");
    setvbuf(LOG_FP, NULL, _IOLBF, BUFSIZ);
    return 0;
}

void logger( int level, char *fmt, ...) {
    if (level <= LOG_LEVEL) {
        va_list argptr;
        va_start(argptr, fmt);
        vprintf( fmt, argptr);
        va_start(argptr, fmt);
        vsprintf( log_buf, fmt, argptr);
        fwrite( log_buf, sizeof(char), strlen(log_buf), LOG_FP);
        va_end(argptr);
    }
}

/* 取子串的函数 */
char* substr(const char*str, unsigned start, unsigned end) {
    unsigned n = end - start;
    static char stbuf[1024];
    strncpy(stbuf, str + start, n); 
    stbuf[n] = 0;
    return stbuf;
}

/* 获取路径的目录名*/
char* dirname(char *full_path) {
    int32_t end_point = strlen(full_path);
    while (true) {
        end_point--;
        if (full_path[end_point] == '/') {
            return substr(full_path, 0, end_point);
        }
    }
}

/* 获取路径的文件名*/
char* basename(char *full_path) {
    int32_t begin_point = strlen(full_path);
    int32_t end_point = strlen(full_path);
    while (true) {
        begin_point--;
        if (full_path[begin_point] == '/') {
            return substr(full_path, begin_point+1, end_point);
        }
    }
}

/* 正则匹配封装函数*/
int32_t regex_match(regmatch_t *pm, char *str, char *pattern) {
    /*
     * 使用示例；
     *  regmatch_t pm[SUB_STR_SIZE];
     *  if(regex_match((regmatch_t *)&pm, line_buf, pattern)) {
     *    for (int x = 1; x < SUB_STR_SIZE && pm[x].rm_so != -1; ++x) {
     *      cout << substr(line_buf, pm[x].rm_so, pm[x].rm_eo) << endl;
     *    }
     *  }
     */
    regex_t reg;
    const size_t nmatch = SUB_STR_SIZE;
    char err_buf[BUF_SIZE];
    int32_t z, cflags = 0, rflag = 1;
    z = regcomp(&reg, pattern, cflags);
    if (z != 0) {
        regerror(z, &reg, err_buf, sizeof(err_buf));
        fprintf(stderr, "%s: pattern '%s' \n", err_buf, pattern);
        rflag = 0;
    }
    z = regexec(&reg, str, nmatch, pm, 0);
    if (z == REG_NOMATCH) {
        rflag = 0;
    } else if (z != 0) {
        regerror(z, &reg, err_buf, sizeof(err_buf));
        fprintf(stderr, "%s: regcom('%s')\n", err_buf, str);
        rflag = 0;
    }
    regfree(&reg);
    return rflag;
}

/* 把curl获取的数据写入文件中*/
size_t WriteFileCallback(void *contents, size_t size, size_t nmemb, void *dst_path)
{
    size_t realsize = size * nmemb;
    FILE *fp = fopen((char *) dst_path, "a");
    fwrite(contents, 1, realsize, fp);
    fclose(fp);
    return realsize;
}

/* 获取internet上的文件到本地*/
bool get_file_to(char *src_url, char *dst_path) {
    unlink(dst_path);
    CURL *curl_handle;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    /* init the curl session */
    curl_handle = curl_easy_init();
    /* specify URL to get */
    cout << src_url << endl;
    curl_easy_setopt(curl_handle, CURLOPT_URL, src_url);
    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteFileCallback);
    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) dst_path);
    /* some servers don't like requests that are made without a user-agent
     *      *      field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    /* get it! */
    res = curl_easy_perform(curl_handle);
    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);
    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();
    /* check for errors */
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return false;
    }
    else {
        return true;
    }
}

void get_file_in_list_to(char *base_url, char *list_path) {
    char line_buf[BUF_SIZE], *tmp_str;
    vector<string> v_file_list;
    ifstream in_file_list (list_path);
    if (!in_file_list) {
        cout << "Unable to open " << list_path << endl;
        exit(1);
    }
    while(!in_file_list.eof()) {
        in_file_list.getline(line_buf, BUF_SIZE);
        if (0 == strlen(line_buf)) {
            continue;
        }
        v_file_list.push_back((string) line_buf);
    }
    in_file_list.close();
    for (vector<string>::iterator v_char_it = v_file_list.begin(); v_char_it != v_file_list.end(); ++v_char_it) {
        v_char_it->insert(0, "/");
        string file_url = ((string) base_url).append(*v_char_it);
        string file_dst = ((string) dirname(list_path)).append(*v_char_it);
        get_file_to((char *) file_url.data(), (char *) file_dst.data());
    }
}

string UrlEncode(const string& szToEncode) {
    std::string src = szToEncode;
    char hex[] = "0123456789ABCDEF";
    string dst;
    for (size_t i = 0; i < src.size(); ++i) {
        unsigned char cc = src[i];
        if (isascii(cc)) {
            if (cc == ' ') {
                dst += "%20";
            } else {
                dst += cc;
            }
        } else {
            unsigned char c = static_cast<unsigned char>(src[i]);
            dst += '%';
            dst += hex[c / 16];
            dst += hex[c % 16];
        }
    }
    return dst;
}

string UrlDecode(const std::string& szToDecode) {
    string result;
    int hex = 0;
    for (size_t i = 0; i < szToDecode.length(); ++i) {
        switch (szToDecode[i]) {
            case '+':
                result += ' ';
                break;
            case '%':
                if (isxdigit(szToDecode[i + 1]) && isxdigit(szToDecode[i + 2])) {
                    std::string hexStr = szToDecode.substr(i + 1, 2);
                    hex = strtol(hexStr.c_str(), 0, 16);
                    //字母和数字[0-9a-zA-Z]、一些特殊符号[$-_.+!*'(),] 、以及某些保留字[$&+,/:;=?@]
                    //可以不经过编码直接用于URL
                    if (!((hex >= 48 && hex <= 57) ||   //0-9
                                (hex >=97 && hex <= 122) || //a-z
                                (hex >=65 && hex <= 90) ||  //A-Z
                                //一些特殊符号及保留字[$-_.+!*'(),]  [$&+,/:;=?@]
                                hex == 0x21 || hex == 0x24 || hex == 0x26 || hex == 0x27 || hex == 0x28 || hex == 0x29
                                || hex == 0x2a || hex == 0x2b|| hex == 0x2c || hex == 0x2d || hex == 0x2e || hex == 0x2f
                                || hex == 0x3A || hex == 0x3B|| hex == 0x3D || hex == 0x3f || hex == 0x40 || hex == 0x5f
                    )) {
                        result += char(hex);
                        i += 2;
                    } else {
                        result += '%';
                    }
                } else {
                    result += '%';
                }
                break;
            default:
                result += szToDecode[i];
                break;
        }
    }
    return result;
}

static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

string base64_encode(char const* bytes_to_encode, unsigned int in_len) {
    string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for(i = 0; (i <4) ; i++) {
                ret += base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }
    if (i) {
        for(j = i; j < 3; j++) {
            char_array_3[j] = '\0';
        }
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        for (j = 0; (j < i + 1); j++) {
            ret += base64_chars[char_array_4[j]];
        }
        while((i++ < 3)) {
            ret += '=';
        }
    }
    return ret;
}

string base64_decode(string const& encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    string ret;
    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++) {
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            }
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            for (i = 0; (i < 3); i++) {
                ret += char_array_3[i];
            }
            i = 0;
        }
    }
    if (i) {
        for (j = i; j <4; j++) {
            char_array_4[j] = 0;
        }
        for (j = 0; j <4; j++) {
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        }
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        for (j = 0; (j < i - 1); j++) {
            ret += char_array_3[j];
        }
    }
    return ret;
}

int32_t catch_crash() {
    struct rlimit rlim = {RLIM_INFINITY,RLIM_INFINITY};
    if (0 != setrlimit(RLIMIT_CORE, &rlim))
    {
    }
    #ifdef __linux__
    int fp = open("/proc/sys/kernel/core_pattern", O_WRONLY);
    if(fp < 0)
    {
        return 1;
    }
    const char * const str_core_pattern = "core.%e.%p";
    if(write(fp, str_core_pattern, strlen(str_core_pattern)) < 0)
    {
        close(fp);
        return 1;
    }
    close(fp);
    #endif
}
