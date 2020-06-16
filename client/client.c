//
// Created by hughnian on 2020/5/27.
//

#include "client.h"
#include "rw.h"
#include "hash.h"

Client *
NewClient(char *addr, int port)
{
    Client *c;
    c = make_new(Client, 1);
    if(c == NULL) {
        return NULL;
    }
    memset(c, 0, sizeof(Client));

    c->port = port;
    c->addr = addr;
    struct sockaddr_in caddr;
    memset(&caddr, 0, sizeof(caddr));
    caddr.sin_family = AF_INET;
    caddr.sin_port   = htons(c->port);
    caddr.sin_addr.s_addr = inet_addr(c->addr);
    c->caddr = caddr;

    c->respHandles = hash_alloc(MAP_SIZE);

    return c;
}

static void
rw_handler(struct ev_loop *el, struct ev_io *watcher, int revents)
{
    Client *c = watcher->data;

    if(revents & EV_ERROR) {
        return;
    }

    if(revents & EV_WRITE) {
        if(WriteNmidReq(c) == 0) {
            perror("client write error\n");
            return;
        }

        ev_io_stop(el, &c->watcher);
        ev_io_init(&c->watcher, rw_handler, c->cfd, EV_READ);
        ev_io_start(el, &c->watcher);
    }

    if(revents & EV_READ) {
        if(ReadNmidRes(c) == 0) {
            perror("client read error\n");
            return;
        }

        Handler *callback;
        hash_remove(c->respHandles, c->Req->handle, (void **)&callback);
        //make_free(callback);
        FreeRequest(c->Req);
        FreeResponse(c->Res);

        ev_io_stop(el, &c->watcher);
        ev_break (el, EVBREAK_ALL);
    }
}

int
Connect(Client *c)
{
    if(NULL == c) {
        return 0;
    }

    c->cfd = socket(AF_INET, SOCK_STREAM, 0);
    if(c->cfd < 0) {
        perror("client socket err\n");
        return 0;
    }
    if(connect(c->cfd, (struct sockaddr*)&(c->caddr), sizeof(c->caddr)) == -1) {
        perror("client connect err\n");
        return 0;
    }

    int flags;
    flags = fcntl(c->cfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(c->cfd, F_SETFL, flags);

    return 1;
}

int
Do(Client *c, char *funcName, char *params, size_t params_size, RespHandler respHandler)
{
    assert(c);

    c->Req = NewRequest();
    if(InitRequest(c->Req, funcName, params, params_size) == 0) {
        return 0;
    }
    c->Res = NewResponse();

    //Handler *callback = make_new(Handler, 1);
    //callback->funcName = funcName;
    //callback->func = respHandler;
    c->Res->callback->funcName = funcName;
    c->Res->callback->func = respHandler;
    if(hash_insert(c->respHandles, funcName, c->Res->callback) < 0) {
        return 0;
    }

    c->loop = ev_loop_new(0);
    c->watcher.data = c;
    ev_io_init(&c->watcher, rw_handler, c->cfd, EV_WRITE);
    ev_io_start(c->loop, &c->watcher);
    ev_run(c->loop, 0);
}

void
Close(Client *c)
{
    assert(c);

    hash_destroy(c->respHandles, NULL);
    c->loop = NULL;
    close(c->cfd);
    make_free(c);
}