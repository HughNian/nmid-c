//
// Created by ADMIN on 2020/4/13.
//

#include "../../worker/common.h"
#include "../../worker/worker.h"
#include "../../worker/agent.h"
#include "../../worker/rw.h"
#include "../../worker/function.h"

#define HOST "192.168.1.176"
#define PORT 6808

char *
str2upper(char *str)
{
    if(str == NULL) return NULL;

    int len = strlen(str);
    char *p;
    p = (char *)malloc(len+1);
    int i = 0;
    while(*str && i != len) {
        *(p+i) = toupper(*str);
        str++;
        i++;
    }
    *(p+i) = '\0';

    return p;
}

RetStruct *
ToUpper(Response *Res)
{
    assert(Res);

    printf("do ToUpper function here\n");
    printf("params %s\n", Res->paramsArr[0]);
    printf("params size %d\n", sizeof(Res->paramsArr[0]));
    printf("params len %d\n", strlen(Res->paramsArr[0]));

    int len = strlen(Res->paramsArr[0]);
    char *data = str2upper(Res->paramsArr[0]);
    printf("upper params %s\n", data);

    RetStruct *ret = NewRetStruct();
    ret->code = 0;
    ret->msg  = "OK";
    ret->data = data;

    return ret;
}

RetStruct *
GetOrderInfo(Response *Res)
{
    assert(Res);

    printf("do GetOrderInfo function here\n");

    RetStruct *ret = NewRetStruct();
    ret->code = 0;
    ret->msg  = "OK";
    ret->data = "good goods";

    return ret;
}

RetStruct *
PostInfo(Response *Res)
{
    assert(Res);

    printf("do PostInfo function here\n");

    RetStruct *ret = NewRetStruct();
    ret->code = 0;
    ret->msg  = "OK";
    ret->data = "add ok";

    return ret;
}

int
main(int argc, char **argv)
{
    Worker *w = NewWorker();
    if(NULL == w) {
        perror("new worker err\n");
        return 0;
    }

    if(AddServer(w, HOST, PORT) == 0) {
        perror("worker add server err\n");
        return 0;
    }

    AddFunction(w, "ToUpper", ToUpper);
    AddFunction(w, "GetOrderInfo", GetOrderInfo);
    AddFunction(w, "PostInfo", PostInfo);
    DelFunction(w, "GetOrderInfo");

    if(Ready(w) == 0) {
        perror("worker ready err\n");
        return 0;
    }

    if(Work(w) == 0) {
        perror("worker work err\n");
        return 0;
    }

    return 1;
}