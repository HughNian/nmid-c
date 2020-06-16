//
// Created by hughnian on 2020/5/27.
//

#include "rw.h"
#include "client.h"

DataBuf *
NewData(size_t size)
{
    if(size <= 0) {
        size = BUF_SIZE;
    }

    DataBuf *data;
    data = make_new(DataBuf,1);
    if(NULL == data) {
        return NULL;
    }
    memset(data, 0, sizeof(DataBuf));

    data->rbytes = 0;
    data->size = size;
    data->buf = malloc(size);
    memset(data->buf, 0, size);

    return data;
}

DataBuf *
ReNewData(DataBuf *data, size_t size)
{
    assert(data);

    if(size <= 0) {
        size = data->size * 2;
    }

    data->size = size;
    void *new_buf;
    new_buf = realloc(data->buf, size);
    data->buf = new_buf;

    return data;
}

void
FreeData(DataBuf *data)
{
    assert(data);

    data->size = 0;
    data->rbytes = 0;
    free(data->buf);
    data->buf = NULL;
    make_free(data);
}

HeaderData *
NewHeader()
{
    HeaderData *hdata;
    hdata = make_new(HeaderData, 1);
    if(NULL == hdata) {
        return NULL;
    }
    memset(hdata, 0, sizeof(HeaderData));

    hdata->clientType = CONN_TYPE_CLIENT;

    return hdata;
}

void
FreeHeader(HeaderData *hdata)
{
    assert(hdata);

    hdata->clientType = 0;
    hdata->dataType = 0;
    hdata->dataLen  = 0;
    make_free(hdata);
}

Request *
NewRequest()
{
    Request *Req;
    Req = make_new(Request, 1);
    if(NULL == Req) {
        return NULL;
    }
    memset(Req, 0, sizeof(Request));

    Req->hdata = NewHeader();
    Req->handleLen  = 0;
    Req->paramsType = PARAMS_TYPE_ONE;
    Req->paramsLen  = 0;
    Req->rdataLen   = 0;

    return Req;
}

void
FreeRequest(Request *Req)
{
    assert(Req);

    FreeHeader(Req->hdata);
    free(Req->rdata);
    make_free(Req);
}

Response *
NewResponse()
{
    Response *Res;
    Res = make_new(Response, 1);
    if(NULL == Res) {
        return NULL;
    }
    memset(Res, 0, sizeof(Response));

    Res->hread = 0; //header is read: 0-no 1-yes
    Res->rdata = NewData(MIN_DATA_SIZE);
    Res->handleLen  = 0;
    Res->paramsType = PARAMS_TYPE_ONE;
    Res->paramsNum  = 0;
    Res->TotalLen   = 0;
    Res->retLen     = 0;
    Res->handle = NULL;
    Res->params = NULL;
    Res->resultData = NULL;
    Res->callback = make_new(Handler, 1);

    return Res;
}

void
FreeResponse(Response *Res)
{
    assert(Res);

    free(Res->handle);
    FreeData(Res->rdata);
    if(Res->dataType == PDT_S_RETURN_DATA) {
        free(Res->params);
        free(Res->resultData->msg);
        free(Res->resultData->data);
        free(Res->resultData);
    }
    make_free(Res->callback);
    Res->hread = 0;
    Res->handle = NULL;
    Res->params = NULL;
    Res->hdata = NULL;
    Res->rdata = NULL;
    Res->callback = NULL;
    make_free(Res);
}

int
ContentPack(Request *Req)
{
    assert(Req);

    if(NULL == Req || Req->hdata == NULL || Req->hdata->clientType != CONN_TYPE_CLIENT) {
        return 0;
    }

    Req->rdataLen = UINT32_SIZE + Req->handleLen + UINT32_SIZE + Req->paramsLen;
    Req->rdata = (char *)malloc(Req->rdataLen);
    memset(Req->rdata, 0, Req->rdataLen);

    uint32_t handleLen = htonl(Req->handleLen);
    memcpy(Req->rdata, &handleLen, UINT32_SIZE);
    memcpy(Req->rdata+UINT32_SIZE, Req->handle, Req->handleLen);
    uint32_t paramsLen = htonl(Req->paramsLen);
    memcpy(Req->rdata+UINT32_SIZE+Req->handleLen, &paramsLen, UINT32_SIZE);
    memcpy(Req->rdata+UINT32_SIZE+Req->handleLen+UINT32_SIZE, Req->params, Req->paramsLen);

    Req->hdata->clientType = htonl(CONN_TYPE_CLIENT);
    Req->hdata->dataType = htonl(PDT_C_DO_JOB);
    Req->hdata->dataLen  = htonl(Req->rdataLen);

    return 1;
}

int
InitRequest(Request *Req, char *handle, char *params, size_t params_size)
{
    assert(Req);

    Req->handle = handle;
    Req->handleLen = strlen(handle);
    Req->params = params;
    Req->paramsLen = params_size;

    if(ContentPack(Req) == 0) {
        return 0;
    }

    return 1;
}

int
WriteNmidReq(Client *c)
{
    assert(c);

    int writeNum, headRet = 0;

    headRet = write(c->cfd, c->Req->hdata, sizeof(HeaderData));
    if(headRet) {
        writeNum = write(c->cfd, c->Req->rdata, c->Req->rdataLen);
        if(writeNum == c->Req->rdataLen) {
            return 1;
        }
    }

    return 0;
}

RetStruct *
msgpack_decode(const char *buf, size_t len)
{
    msgpack_unpacked result;
    size_t off = 0;
    msgpack_unpack_return ret;
    int i = 0;
    RetStruct *ret_struct = (RetStruct *)malloc(sizeof(RetStruct));
    memset(ret_struct, 0, sizeof(RetStruct));

    msgpack_unpacked_init(&result);
    ret = msgpack_unpack_next(&result, buf, len, &off);
    while (ret == MSGPACK_UNPACK_SUCCESS) {
        msgpack_object obj = result.data;

        int i = 0;
        if(obj.type == MSGPACK_OBJECT_ARRAY) {
            int size = obj.via.array.size;
            for(; i < size; i++) {
                if(obj.via.array.ptr[i].type == MSGPACK_OBJECT_POSITIVE_INTEGER) {
                    ret_struct->code = obj.via.array.ptr[i].via.u64;
                } else if(obj.via.array.ptr[i].type == MSGPACK_OBJECT_STR) {
                    if(i == 1) {
                        int size = obj.via.array.ptr[i].via.str.size;
                        char *msg = (char *)malloc(size+1);
                        memcpy(msg, obj.via.array.ptr[i].via.str.ptr, size);
                        msg[size] = '\0';
                        ret_struct->msg = msg;
                    }
                    if(i == 2) {
                        int size = obj.via.array.ptr[i].via.str.size;
                        char *data = (char *)malloc(size+1);
                        memcpy(data, obj.via.array.ptr[i].via.str.ptr, size);
                        data[size] = '\0';
                        ret_struct->data = data;
                    }
                }
            }
        } else if(obj.type == MSGPACK_OBJECT_MAP) {
            int size = obj.via.map.size;
            size *= 2;
            for(; i < size; i++) {
                if(i % 2 != 0) {
                    if(obj.via.array.ptr[i].type == MSGPACK_OBJECT_POSITIVE_INTEGER) {
                        ret_struct->code = obj.via.array.ptr[i].via.u64;
                    } else if(obj.via.array.ptr[i].type == MSGPACK_OBJECT_STR) {
                        int size = obj.via.array.ptr[i].via.str.size;
                        char *msg = (char *)malloc(size+1);
                        memcpy(msg, obj.via.array.ptr[i].via.str.ptr, size);
                        msg[size] = '\0';
                        ret_struct->msg = msg;
                    } else if(obj.via.array.ptr[i].type == MSGPACK_OBJECT_BIN) {
                        int size = obj.via.array.ptr[i].via.str.size;
                        char *data = (char *)malloc(size+1);
                        memcpy(data, obj.via.array.ptr[i].via.str.ptr, size);
                        data[size] = '\0';
                        ret_struct->data = data;
                    }
                }
            }
        }

        ret = msgpack_unpack_next(&result, buf, len, &off);
    }
    msgpack_unpacked_destroy(&result);

    return ret_struct;
}

int
ReadNmidRes(Client *c)
{
    assert(c);

    int readNum = 0;
    for(;;) {
        readNum = read(c->cfd, c->Res->rdata->buf, c->Res->rdata->size);
        if(readNum == 0) {
            perror("connected closed!\n");
            return 0;
        } else if(readNum == -1) {
            if (errno == EINTR || errno == EWOULDBLOCK) {
                continue;
            }
            perror("read error!\n");
            return 0;
        } else {
            c->Res->rdata->rbytes = readNum;

            if(readNum < MIN_DATA_SIZE) {
                continue;
            } else if(c->Res->hread == 0 && readNum == MIN_DATA_SIZE) { //read header
                c->Res->hread = 1;
                c->Res->hdata = (HeaderData *)c->Res->rdata->buf;

                if(ntohl(c->Res->hdata->clientType) != CONN_TYPE_SERVER) {
                    perror("server response illegal\n");
                    return 0;
                }

                c->Res->dataType = ntohl(c->Res->hdata->dataType);
                if(c->Res->dataType == PDT_S_RETURN_DATA) {
                    c->Res->TotalLen = ntohl(c->Res->hdata->dataLen);
                    size_t new_size = c->Res->TotalLen;
                    c->Res->rdata = ReNewData(c->Res->rdata, new_size);

                    continue;
                } else {
                    c->Res->handle = strdup(c->Req->handle);
                    break;
                }
            } else if(c->Res->hread == 1) { //read body
                GetDataStruct *getData = (GetDataStruct *)c->Res->rdata->buf;
                uint32_t handleLen = getData->handleLen;
                c->Res->handleLen = ntohl(handleLen);

                uint32_t paramsLen = getData->paramsLen;
                c->Res->paramsLen = ntohl(paramsLen);

                uint32_t retLen = getData->retLen;
                c->Res->retLen = ntohl(retLen);

                //handle
                char *leftChar = (char *)getData+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint32_t);
                c->Res->handle = (char *)malloc(c->Res->handleLen+1);
                memset(c->Res->handle, 0, c->Res->handleLen+1);
                strncpy(c->Res->handle, leftChar, c->Res->handleLen);
                c->Res->handle[c->Res->handleLen] = '\0';

                //params
                leftChar += c->Res->handleLen;
                c->Res->params = (char *)malloc(c->Res->paramsLen+1);
                memset(c->Res->params, 0, c->Res->paramsLen+1);
                strncpy(c->Res->params, leftChar, c->Res->paramsLen);
                c->Res->params[c->Res->paramsLen] = '\0';

                //ret
                leftChar += c->Res->paramsLen;
                c->Res->ret = leftChar;

                break;
            }
        }
    }

    //return data
    Handler *callback;
    if(c->Res->dataType == PDT_S_RETURN_DATA) {
        hash_lookup(c->respHandles, c->Res->handle, (void **)&callback);
        if(strcmp(callback->funcName, c->Res->handle) == 0 && strcmp(c->Req->handle, c->Res->handle) == 0) {
            /*msgpack_zone mempool;
            msgpack_zone_init(&mempool, c->Res->retLen*2);
            msgpack_object deserialized;
            msgpack_unpack(c->Res->ret, c->Res->retLen, NULL, &mempool, &deserialized);
            msgpack_object_print(stdout, deserialized);
            printf("\n");*/

            c->Res->resultData = msgpack_decode(c->Res->ret, c->Res->retLen);

            callback->func(c->Res);
        }
    } else {
        hash_lookup(c->respHandles, c->Res->handle, (void **)&callback);
        if(strcmp(callback->funcName, c->Res->handle) == 0) {
            callback->func(c->Res);
        }
    }

    return 1;
}