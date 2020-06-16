#ifndef LIBNMID_WORKER_RW_H
#define LIBNMID_WORKER_RW_H

#include "common.h"

struct _HeaderData {
    uint32_t clientType;
    uint32_t dataType;
    uint32_t dataLen;
};

struct _DataBuf {
    size_t rbytes;
    size_t size;
    void *buf;
};

struct _RetStruct {
    int code;
    char *msg;
    char *data;
};

struct _GetDataStruct {
    uint32_t handleLen;
    uint32_t paramsLen;
};

typedef struct _GetDataStruct GetDataStruct;

//请求nmid-server
struct _Request {
    //struct member var
    Agent *a;

    HeaderData *hdata;
    RetStruct  *reqRetData;

    uint32_t dataType;
    size_t handleLen;
    uint32_t paramsType;
    uint32_t paramsNum;
    size_t paramsLen;
    size_t retLen;
    uint dataLen;
    char *handle;
    char *params;
    msgpack_sbuffer sbuf;
    char *ret;  //结果数据
    char *ret_data; //包含结果数据的返回数据

    //method pointer
    int (*GrabJob)(Request *Req);
    int (*AddFunctionPack)(Request *Req, char *funcName);
    int (*DelFunctionPack)(Request *Req, char *funcName);
    int (*WriteNmidReq)(Request *Req);
};

//nmid-server返回
struct _Response {
    //struct member var
    Agent *a;

    uint hread:1; //header is read: 0-no 1-yes
    HeaderData  *hdata;
    DataBuf *rdata;

    uint32_t dataType;
    uint32_t handleLen;
    uint32_t paramsType;
    size_t paramsLen;
    int paramsNum;
    size_t TotalLen;
    uint32_t retLen;
    char *handle;
    char *params;
    char **paramsArr;

    //method pointer
    int (*ReadNmidRes)(Response *Res);
};

DataBuf *NewData(size_t size);

DataBuf *ReNewData(DataBuf *data, size_t size);

void FreeData(DataBuf *data);

Request *NewRequest(Agent *a);

void FreeRequest(Request *Req);

Response *NewResponse(Agent *a);

RetStruct *NewRetStruct();

int GrabJob(Request *Req);

int AddFunctionPack(Request *Req, char *funcName);

int DelFunctionPack(Request *Req, char *funcName);

int ReadNmidRes(Response *Res);

int WriteNmidReq(Request *Req);

int DecodePack(Response *Res);

void WorkerDo(Response *Res);

#endif //LIBNMID_WORKER_RW_H
