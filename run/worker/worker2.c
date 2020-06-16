//
// Created by ADMIN on 2020/4/28.
//

#include <nmidworker.h>

#define HOST "192.168.1.176"
#define PORT 6808

RetStruct *
ToUpper(Response *Res)
{
    assert(Res);

    printf("do ToUpper function here\n");

    StrtoUpper(Res->paramsArr[0]);

    RetStruct *ret = NewRetStruct();
    ret->code = 0;
    ret->msg  = "OK";
    ret->data = Res->paramsArr[0];

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

//    AddFunction(w, "Upper", ToUpper);
    AddFunction(w, "GetOrderInfo", GetOrderInfo);

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