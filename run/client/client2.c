//
// Created by ADMIN on 2020/4/13.
//

#include <nmidclient.h>

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