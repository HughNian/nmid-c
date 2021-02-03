#include "rw.h"
#include "agent.h"
#include "worker.h"
#include "function.h"
#include "util.h"

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

    hdata->clientType = htonl(CONN_TYPE_WORKER);

    return hdata;
}

void
FreeHeader(HeaderData *hdata)
{
    assert(hdata);

    hdata->dataType = 0;
    hdata->dataLen  = 0;
    hdata->clientType = 0;
    make_free(hdata);
}

Request *
NewRequest(Agent *a)
{
    assert(a);

    Request *Req;
    Req = make_new(Request, 1);
    if(NULL == Req) {
        return NULL;
    }
    memset(Req, 0, sizeof(Request));

    Req->a = a;

    HeaderData *Hdata;
    Hdata = make_new(HeaderData, 1);
    if(NULL == Hdata) {
        make_free(Req);
        return NULL;
    }
    memset(Hdata, 0, sizeof(HeaderData));
    Req->hdata = Hdata;
    Req->hdata->clientType = htonl(CONN_TYPE_WORKER);

    Req->handleLen  = 0;
    Req->paramsType = PARAMS_TYPE_ONE;
    Req->paramsNum  = 0;
    Req->paramsLen  = 0;

    Req->GrabJob = GrabJob;
    Req->AddFunctionPack = AddFunctionPack;
    Req->DelFunctionPack = DelFunctionPack;
    Req->WriteNmidReq = WriteNmidReq;

    return Req;
}

void
FreeRequest(Request *Req)
{
    make_free(Req->hdata);
    make_free(Req);
}

Response *
NewResponse(Agent *a)
{
    assert(a);

    Response *Res;
    Res = make_new(Response, 1);
    if(NULL == Res) {
        return NULL;
    }
    memset(Res, 0, sizeof(Response));

    Res->a = a;

    Res->hread = 0;
    HeaderData *Hdata;
    Hdata = make_new(HeaderData, 1);
    if(NULL == Hdata) {
        make_free(Res);
        return NULL;
    }
    memset(Hdata, 0, sizeof(HeaderData));
    Res->hdata = Hdata;
    Res->rdata = NULL;

    Res->handleLen  = 0;
    Res->paramsType = PARAMS_TYPE_ONE;
    Res->paramsNum  = 0;
    Res->TotalLen  = 0;
    Res->handle = NULL;
    Res->params = NULL;

    Res->ReadNmidRes = ReadNmidRes;

    return Res;
}

RetStruct *
NewRetStruct()
{
    RetStruct *ret_str;
    ret_str = make_new(RetStruct, 1);
    if(NULL == ret_str) {
        return NULL;
    }
    memset(ret_str, 0, sizeof(RetStruct));

    return ret_str;
}

int
GrabJob(Request *Req)
{
    printf("grab job here\n");

    if(NULL == Req || Req->hdata == NULL || ntohl(Req->hdata->clientType) != CONN_TYPE_WORKER) {
        return 0;
    }

    Req->hdata->dataType = htonl(PDT_W_GRAB_JOB);
    Req->hdata->dataLen  = htonl(1);
    Req->handle = "1";  //todo modify
    Req->handleLen = 1;

    return 1;
}

int
AddFunctionPack(Request *Req, char *funcName)
{
    printf("add function here\n");

    if(NULL == Req || Req->hdata == NULL || ntohl(Req->hdata->clientType) != CONN_TYPE_WORKER) {
        return 0;
    }

    if(NULL == funcName || strlen(funcName) == 0) {
        return 0;
    }

    Req->hdata->dataType = htonl(PDT_W_ADD_FUNC);
    Req->hdata->dataLen  = htonl(strlen(funcName));
    Req->handle = funcName;
    Req->handleLen = strlen(funcName);

    return 1;
}

int
DelFunctionPack(Request *Req, char *funcName)
{
    printf("del function here\n");

    if(NULL == Req || Req->hdata == NULL || ntohl(Req->hdata->clientType) != CONN_TYPE_WORKER) {
        return 0;
    }

    if(NULL == funcName || strlen(funcName) == 0) {
        return 0;
    }

    Req->hdata->dataType = htonl(PDT_W_DEL_FUNC);
    Req->hdata->dataLen  = htonl(strlen(funcName));
    Req->handle = funcName;
    Req->handleLen = strlen(funcName);

    return 1;
}

int
RetDataPack(Request *Req)
{
    if(NULL == Req || Req->hdata == NULL || ntohl(Req->hdata->clientType) != CONN_TYPE_WORKER) {
        return 0;
    }

    size_t ret_size;
    Req->ret = ReqMsgPackEncode(Req->reqRetData, &ret_size, &(Req->sbuf));
    Req->retLen = ret_size;
    Req->dataLen = UINT32_SIZE + Req->handleLen + UINT32_SIZE + Req->paramsLen + UINT32_SIZE + Req->retLen;
    Req->ret_data = (char *)malloc(Req->dataLen);
    memset(Req->ret_data, 0, Req->dataLen);
    uint32_t handleLen = htonl(Req->handleLen);
    memcpy(Req->ret_data, &handleLen, UINT32_SIZE);
    memcpy(Req->ret_data+UINT32_SIZE, Req->handle, Req->handleLen);
    uint32_t paramsLen = htonl(Req->paramsLen);
    memcpy(Req->ret_data+UINT32_SIZE+Req->handleLen, &paramsLen, UINT32_SIZE);
    memcpy(Req->ret_data+UINT32_SIZE+Req->handleLen+UINT32_SIZE, Req->params, Req->paramsLen);
    uint32_t retLen = htonl(Req->retLen);
    memcpy(Req->ret_data+UINT32_SIZE+Req->handleLen+UINT32_SIZE+Req->paramsLen, &retLen, UINT32_SIZE);
    memcpy(Req->ret_data+UINT32_SIZE+Req->handleLen+UINT32_SIZE+Req->paramsLen+UINT32_SIZE, Req->ret, Req->retLen);

    Req->dataType = PDT_W_RETURN_DATA;
    Req->hdata->dataType = htonl(PDT_W_RETURN_DATA);
    Req->hdata->dataLen  = htonl(Req->dataLen);

    return 1;
}

int
DecodePack(Response *Res)
{
    if(NULL == Res) {
        return 0;
    }

    ResMsgPackArrDecode(Res->params, Res->paramsNum);
    char *unpack_buf = ResMsgPackDecode(Res->params, Res->paramsLen);

    uint32_t paramsNum = GetParamsNum(unpack_buf);
    if(paramsNum == 0) {
        perror("params num error\n");
        return 0;
    }
    Res->paramsNum = paramsNum;

    if(Res->paramsNum == 1) {
        Res->paramsType = PARAMS_TYPE_ONE;
    } else if(Res->paramsNum > 1) {
        Res->paramsType = PARAMS_TYPE_MUL;
    }

    char **paramsArr = GetParamsArr(unpack_buf, Res->paramsNum);

    free(unpack_buf);
    if(NULL == paramsArr) {
        perror("params arr get error\n");
        return 0;
    }

    Res->paramsArr = paramsArr;

    return 1;
}

void
WorkerDo(Response *Res)
{
    if(NULL == Res) {
        return;
    }

    if(!DecodePack(Res)) {
        perror("response decode pack err\n");
        return;
    }

    //get function
    Func *function = GetFunc(Res->a->w->funcs, Res->handle);
    if(NULL == function) {
        perror("get func err\n");
        return;
    }
    if(strcmp(Res->handle, function->funcName) != 0) {
        perror("func name not equal\n");
        return;
    }
    uint hashval = Hash(Res->handle, strlen(Res->handle));
    if(hashval != function->hashval) {
        perror("func hash not equal\n");
        return;
    }

    //function do
    RetStruct *ret = function->func(Res);
    Res->a->Req->reqRetData = ret;
    Res->a->Req->handleLen  = Res->handleLen;
    Res->a->Req->paramsType = Res->paramsType;
    Res->a->Req->paramsNum  = Res->paramsNum;
    Res->a->Req->paramsLen  = Res->paramsLen;
    Res->a->Req->handle     = strdup(Res->handle);
    Res->a->Req->params     = strdup(Res->params);

    RetDataPack(Res->a->Req);

    return;
}

int
ReadNmidRes(Response *Res)
{
    if(NULL == Res) {
        return 0;
    }

    //Agent *a = struct_from2(Res, Agent);
    //assert(a);

    if(Res->rdata != NULL) {
        FreeData(Res->rdata);
    }
    Res->rdata = NewData(MIN_DATA_SIZE);

    int readNum = 0;
    for(;;) {
        readNum = read(Res->a->afd, Res->rdata->buf, Res->rdata->size);
        if(readNum == 0) {
            perror("connected closed!");
            Close(Res->a);
            //return 0;
            exit(1); //这里如果nmid服务端退出了，作为worker端则正常退出。//todo 后面打算做成每隔半小时尝试重连
        } else if(readNum == -1) {
            if (errno == EINTR || errno == EWOULDBLOCK) {
                continue;
            }
            perror("read error!");
            Close(Res->a);
	        return 0;
        } else {
            Res->rdata->rbytes = readNum;

            if(readNum < MIN_DATA_SIZE) {
                continue;
            } else if(readNum == MIN_DATA_SIZE) { //get header
                Res->hread = 1;
                Res->hdata = (HeaderData *)Res->rdata->buf;

                if(ntohl(Res->hdata->clientType) != CONN_TYPE_SERVER) {
                    perror("server response illegal\n");
                    Close(Res->a);
                    return 0;
                }
                Res->dataType = ntohl(Res->hdata->dataType);
                printf("server response datatype : %d\n", Res->dataType);
                if(Res->dataType == PDT_S_GET_DATA) {
                    Res->TotalLen = ntohl(Res->hdata->dataLen);
                    size_t new_size = Res->TotalLen;
                    Res->rdata = ReNewData(Res->rdata, new_size);
                }

                continue;
            } else if(Res->hread == 1) { //get data body
                //GetDataStruct *getData = (GetDataStruct *)Res->rdata->buf;
                //uint32_t handleLen = getData->handleLen;
                //Res->handleLen = ntohl(handleLen);
                //if(Res->handle != NULL) Res->handle = NULL;
                //Res->handle = (char *)getData+sizeof(uint32_t);
                //if(Res->params != NULL) Res->params = NULL;
                //Res->params = (char *)getData+sizeof(uint32_t)+Res->handleLen+sizeof(uint32_t);
                //Res->paramsLen = strlen(Res->params);

                //uint32_t handleLen = *(uint32_t *)Res->rdata->buf;
                //Res->handleLen = ntohl(handleLen);
                //Res->handle = (char *)Res->rdata->buf+sizeof(uint32_t);
                //Res->params = (char *)Res->rdata->buf+sizeof(uint32_t)+Res->handleLen+sizeof(uint32_t);
                //Res->paramsLen = strlen(Res->params);

                GetDataStruct *getData = (GetDataStruct *)Res->rdata->buf;
                uint32_t handleLen = getData->handleLen;
                Res->handleLen = ntohl(handleLen);

                uint32_t paramsLen = getData->paramsLen;
                Res->paramsLen = ntohl(paramsLen);

                char *leftChar = (char *)getData+sizeof(uint32_t)+sizeof(uint32_t);
                Res->handle = (char *)malloc(Res->handleLen+1);
                memset(Res->handle, 0, Res->handleLen+1);
                strncpy(Res->handle, leftChar, Res->handleLen);
                Res->handle[Res->handleLen] = '\0';

                leftChar += Res->handleLen;
                Res->params = (char *)malloc(Res->paramsLen+1);
                memset(Res->params, 0, Res->paramsLen+1);
                strncpy(Res->params, leftChar, Res->paramsLen);
                Res->params[Res->paramsLen] = '\0';

                break;
            }
        }
    }

    if(Res->dataType == PDT_S_GET_DATA) {
        WorkerDo(Res);
    }

    //reset
    Res->hread = 0;
    free(Res->handle);
    free(Res->params);
    FreeParamsArr(Res->paramsArr, Res->paramsNum);

    return 1;
}

int
WriteNmidReq(Request *Req)
{
    if(NULL == Req || Req->dataType == 0) {
        return 0;
    }

    int writeNum, headRet = 0;

    headRet = write(Req->a->afd, Req->hdata, sizeof(HeaderData));
    printf("write head num:%d\n", headRet);

    if(Req->dataType == PDT_W_GRAB_JOB ||
       Req->dataType == PDT_W_ADD_FUNC ||
       Req->dataType == PDT_W_DEL_FUNC) {
        printf("write handle : %s\n", Req->handle);
        if(headRet) {
            writeNum = write(Req->a->afd, Req->handle, Req->handleLen);
            printf("write num 1 : %d\n", writeNum);
        }
    } else if(Req->dataType == PDT_W_RETURN_DATA) {
        if(headRet) {
            writeNum = write(Req->a->afd, Req->ret_data, Req->dataLen);
            if(writeNum == Req->dataLen) {
                printf("write num 2 : %d\n", writeNum);
                make_free(Req->reqRetData);
                free(Req->handle);
                free(Req->params);
                msgpack_sbuffer_destroy(&Req->sbuf);
                free(Req->ret_data);
            }
        }
    }

    return writeNum;
}
