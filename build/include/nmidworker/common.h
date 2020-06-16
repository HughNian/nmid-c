#ifndef LIBNMID_WORKER_COMMON_H
#define LIBNMID_WORKER_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <ev.h>
#include <msgpack.h>

typedef unsigned int uint;
typedef void* pointer;
typedef struct _Worker Worker;
typedef struct _Agent Agent;
typedef struct _AgentArr AgentArr;
typedef struct _HeaderData HeaderData;
typedef struct _DataBuf DataBuf;
typedef struct _RetStruct RetStruct;
typedef struct _Request Request;
typedef struct _Response Response;
typedef struct _Func Func;
typedef struct _FuncMap FuncMap;
typedef RetStruct *(*JobFunc)(Response *Res);

enum PACKAGE_DATA_TYPE {
    PDT_OK            = 1,
    PDT_ERROR         = 2,
    PDT_CAN_DO        = 3,
    PDT_CANT_DO       = 4,
    PDT_NO_JOB        = 5,
    PDT_HAVE_JOB      = 6,
    PDT_TOSLEEP       = 7,
    PAT_WAKEUP		  = 8,
    PDT_WAKEUPED      = 9,
    PDT_S_GET_DATA    = 10,
    PDT_S_RETURN_DATA = 11,
    PDT_W_GRAB_JOB    = 12,
    PDT_W_ADD_FUNC    = 13,
    PDT_W_DEL_FUNC    = 14,
    PDT_W_RETURN_DATA = 15,
    PDT_C_DO_JOB      = 16,
    PDT_C_GET_DATA    = 17
};

enum CONN_TYPE {
    CONN_TYPE_SERVER = 1,
    CONN_TYPE_WORKER = 2,
    CONN_TYPE_CLIENT = 3,
    PARAMS_TYPE_ONE  = 4,
    PARAMS_TYPE_MUL  = 5,
};

#define MIN_DATA_SIZE 12
#define UINT32_SIZE 4
#define QUEUE_SIZE 64
#define BUF_SIZE 512
#define ARRAY_SIZE 512
#define MAP_SIZE 128
#define PARAMS_SCOPE 0x3A

#define make_new(struct_type, struct_num) ((struct_type *)malloc(sizeof(struct_type)*struct_num))
#define make_new1(struct_type, struct_num) ((struct_type *)calloc(1, sizeof(struct_type)*struct_num))
#define make_realloc(mem, bytes_size) realloc(mem, bytes_size)
#define make_free(mem) free(mem)
#define struct_from2(memberPtr, structType) \
	((structType*) ((char*) memberPtr - offsetof(structType, memberPtr)))

#endif //LIBNMID_WORKER_COMMON_H
