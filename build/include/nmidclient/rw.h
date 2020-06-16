//
// Created by hughnian on 2020/5/27.
//

#ifndef LIBNMID_CLIENT_RW_H
#define LIBNMID_CLIENT_RW_H

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
    uint32_t retLen;
};

typedef struct _GetDataStruct GetDataStruct;

struct _Request {
    HeaderData *hdata;
    RetStruct  *reqRetData;

    uint32_t dataType;
    size_t handleLen;
    uint32_t paramsType;
    size_t paramsLen;
    uint rdataLen;
    char *handle;
    char *params;
    char *rdata;
};

struct _Response {
    uint hread:1;
    HeaderData *hdata;
    DataBuf *rdata;

    uint32_t dataType;
    size_t handleLen;
    uint32_t paramsType;
    uint32_t paramsNum;
    size_t paramsLen;
    size_t TotalLen;
    size_t retLen;
    char *handle;
    char *params;
    char *ret;

    RetStruct *resultData;

    Handler *callback;
};

DataBuf *NewData(size_t size);

DataBuf *ReNewData(DataBuf *data, size_t size);

void FreeData(DataBuf *data);

HeaderData *NewHeader();

void FreeHeader(HeaderData *hdata);

Request *NewRequest();

int InitRequest(Request *Req, char *handle, char *params, size_t params_size);

void FreeRequest(Request *Req);

Response *NewResponse();

void FreeResponse(Response *Res);

int WriteNmidReq(Client *c);

int ReadNmidRes(Client *c);

#endif //LIBNMID_CLIENT_RW_H
