<div align="center">
    <a href="http://www.niansong.top"><img src="https://raw.githubusercontent.com/HughNian/nmid-c/master/logo/nmid_c_logo.png" alt="nmid Logo" width="160"></a>
</div>

## nmid-c介绍

nmid-c是nmid微服务调度系统的客户端和工作端的C语言实现。同时可以编译成C动态库，在其他C程序中调用。nmid-php-ext作为php的扩展就是用了nmid-c项目的动态库。

1.client目录为客户端C语言源码目录，采用libev用作网络库，nmid的自有I/O通信协议，msgpack作为通信数据格式   

2.worker目录为工作端C语言源码目录，采用libev用作网络库，nmid的自有I/O通信协议，msgpack作为通信数据格式   

3.run目录为客户端，工作端C程序的可执行文件，可执行文件的编译用的是make  

4.build目录为客户端，工作端C程序的动态库目录，动态库的编译用的是cmake   


## 建议配置

```
cat /proc/version
Linux version 3.10.0-957.21.3.el7.x86_64 ...(centos7)

go version
go1.12.5 linux/amd64

gcc --version
gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-36)

cmake --version
cmake version 3.11.4

```

## 需要的库

```
-lpthread, -lev, -lmsgpackc

```

## 编译安装步骤

```
https://github.com/HughNian/nmid-c.git

1.client可执行文件
cd nmid-c/run/client
make

2.client动态库
cd nmid-c/build/client
里面有之前编译好的动态库，可以把它们都删掉，然后重新编译
cmake ../../client
make
make install
需要把安装到/usr/local/lib中的动态库ln -s到/usr/lib64中

3.worker可执行文件
cd nmid-c/run/worker
make

4.worker动态库
cd nmid-c/build/worker
里面有之前编译好的动态库，可以把它们都删掉，然后重新编译
cmake ../../worker
make
make install
需要把安装到/usr/local/lib中的动态库ln -s到/usr/lib64中

```

## 使用

```c
工作端代码

nmid调度服务端ip:192.168.1.176
nmid调度服务端port:6808

#include "../worker/common.h"
#include "../worker/worker.h"
#include "../worker/agent.h"
#include "../worker/rw.h"
#include "../worker/function.h"

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

```

```c
客户端代码

nmid调度服务端ip:192.168.1.176
nmid调度服务端port:6808

#include "../../client/common.h"
#include "../../client/client.h"
#include "../../client/rw.h"

#define HOST "192.168.1.176"
#define PORT 6808

void
msgpack_encode(msgpack_sbuffer *sbuf)
{
    msgpack_sbuffer_init(sbuf);

    msgpack_packer pk;

    msgpack_packer_init(&pk, sbuf, msgpack_sbuffer_write);
    /* 1st object */
    msgpack_pack_array(&pk, 1);
    msgpack_pack_str(&pk, 13);
    msgpack_pack_str_body(&pk, "name:niansong", 13);
}

void
msgpack_encode2(msgpack_sbuffer *sbuf)
{
    msgpack_sbuffer_init(sbuf);

    msgpack_packer pk;

    msgpack_packer_init(&pk, sbuf, msgpack_sbuffer_write);
    /* 1st object */
    msgpack_pack_array(&pk, 3);
    msgpack_pack_str(&pk, 21);
    msgpack_pack_str_body(&pk, "order_sn:MBO993889253", 21);
    msgpack_pack_str(&pk, 12);
    msgpack_pack_str_body(&pk, "order_type:4", 12);
    msgpack_pack_str(&pk, 12);
    msgpack_pack_str_body(&pk, "fenxiao:2288", 12);
}

void
RespCallback(Response *Res)
{
    assert(Res);

    if(Res->dataType == PDT_S_RETURN_DATA) {
        if(Res->resultData->code == 0) {
            printf("ret struct data - %s\n\n", Res->resultData->data);
        } else {
            printf("ret struct msg - %s\n\n", Res->resultData->msg);
        }
    } else if(Res->dataType == PDT_ERROR) {
        printf("request error\n\n");
    } else if(Res->dataType == PDT_CANT_DO) {
        printf("have no job do\n\n");
    }
}

void
RespCallback2(Response *Res)
{
    assert(Res);

    if(Res->dataType == PDT_S_RETURN_DATA) {
        if(Res->resultData->code == 0) {
            printf("ret struct data2 - %s\n\n", Res->resultData->data);
        } else {
            printf("ret struct msg2 - %s\n\n", Res->resultData->msg);
        }
    } else if(Res->dataType == PDT_ERROR) {
        printf("request error2\n\n");
    } else if(Res->dataType == PDT_CANT_DO) {
        printf("have no job do2\n\n");
    }
}

int
main(int argc, char **argv)
{
    Client *c;
    c = NewClient(HOST, PORT);
    if(NULL == c) {
        return 0;
    }

    if(Connect(c) == 0) {
        return 0;
    }

    msgpack_sbuffer sbuf;
    msgpack_encode(&sbuf);
    Do(c, "PostInfo", sbuf.data, sbuf.size, RespCallback);
    Do(c, "PostInfo", sbuf.data, sbuf.size, RespCallback2);
    msgpack_sbuffer_destroy(&sbuf);

    msgpack_sbuffer sbuf2;
    msgpack_encode2(&sbuf2);
    Do(c, "GetOrderInfo", sbuf2.data, sbuf2.size, RespCallback);
    msgpack_sbuffer_destroy(&sbuf2);

    Close(c);

    return 1;
}

```

## 交流博客

http://www.niansong.top