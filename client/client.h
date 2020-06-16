//
// Created by hughnian on 2020/5/27.
//

#ifndef LIBNMID_CLIENT_CLIENT_H
#define LIBNMID_CLIENT_CLIENT_H

#include "common.h"
#include "hash.h"

struct _Client {
    int  cfd;
    int  port;
    char *addr;
    struct sockaddr_in caddr;
    struct ev_loop *loop;
    struct ev_io watcher;
    struct _sHashTable *respHandles;

    Request  *Req;
    Response *Res;
};

struct _Handler {
    char *funcName;
    RespHandler func;
};

Client *NewClient(char *addr, int port);

int Connect(Client *c);

int Do(Client *c, char *funcName, char *params, size_t params_size, RespHandler respHandler);

void Close(Client *c);
#endif //LIBNMID_CLIENT_CLIENT_H
