#include "agent.h"
#include "rw.h"
#include "worker.h"
#include "util.h"

AgentArr *
NewAgentArr(int size)
{
    if(size <= 0) {
        size = ARRAY_SIZE;
    }

    AgentArr *agentArr;
    agentArr = make_new(AgentArr, 1);
    if(NULL == agentArr) {
        return NULL;
    }

    pthread_cond_init(&agentArr->cond, NULL);
    pthread_mutex_init(&agentArr->lock, NULL);
    agentArr->size = size;
    agentArr->cur  = 0;
    agentArr->a = make_new(Agent*, size);
    if(NULL == agentArr->a) {
        make_free(agentArr);
        return NULL;
    }

    return agentArr;
}

int
reNewAgentArr(AgentArr *agentArr)
{
    if(NULL == agentArr) {
        return 0;
    }
    if(agentArr->size == 0) {
        return 0;
    }

    Agent **newAgent;
    int newSize;

    agentArr->size *= 2;
    newSize = sizeof(Agent *) * agentArr->size;
    newAgent = (Agent **)make_realloc(agentArr->a, newSize);
    if(NULL == newAgent) {
        return 0;
    }

    agentArr->a = newAgent;
    memset(agentArr->a[agentArr->cur], 0, (newSize - agentArr->cur));

    return 1;
}

Agent *
NewAgent(Worker *w, char *host, int port)
{
    assert(w);

    if(strlen(host) == 0) {
        return NULL;
    }

    Agent *a;
    a = make_new(Agent, 1);
    if(NULL == a) {
        return a;
    }

    pthread_cond_init(&a->cond, NULL);
    pthread_mutex_init(&a->lock, NULL);

    a->curIndex = -1;
    a->host = host;
    a->port = port;
    a->ready = 0;
    a->running = 0;
    //a->Res  = NewResponse(a);
    a->w = w;

    return a;
}

int
AddAgent(AgentArr *agentArr, Agent *a)
{
    if(NULL == agentArr || NULL == a) {
        return 0;
    }

    pthread_mutex_lock(&(agentArr->lock));
    if(agentArr->cur >= agentArr->size) {
        int ret = reNewAgentArr(agentArr);
        if(!ret) {
            return ret;
        }
    }

    a->curIndex = agentArr->cur;
    agentArr->a[a->curIndex] = a;
    agentArr->cur++;
    pthread_mutex_unlock(&(agentArr->lock));

    return 1;
}

int
DelAgent(AgentArr *agentArr, Agent *a)
{
    if(NULL == agentArr || NULL == a || a->curIndex == -1) {
        return 0;
    }

    pthread_mutex_lock(&(agentArr->lock));
    int i = 0;
    for(; i < agentArr->cur; i++) {
        if(i == a->curIndex) {
            agentArr->cur--;
            int j = i;
            for(; j < agentArr->cur; j++) {
                Agent *temp_a = agentArr->a[j];
                make_free(temp_a);
                agentArr->a[j] = NULL;
                agentArr->a[j] = agentArr->a[j+1];
            }
        }
    }
    pthread_mutex_unlock(&(agentArr->lock));

    return 1;
}

int
Grab(Agent *a)
{
    Request *Req = NewRequest(a);

    if(NULL == a || NULL == Req) {
        return 0;
    }

    Req->dataType = PDT_W_GRAB_JOB;
    printf("grab job dataType : %d\n", Req->dataType);
    Req->GrabJob(Req);
    int ret = Req->WriteNmidReq(Req);
    if(ret) {
        FreeRequest(Req);
    }

    return ret;
}

static void *
broadcast_handler(void *data)
{
    Request *Req = data;

    if(NULL == Req || NULL == Req->a) {
        return NULL;
    }
    printf("broadcast dataType : %d\n", Req->dataType);
    int ret = Req->WriteNmidReq(Req);
    if(ret) {
        FreeRequest(Req);
        pthread_exit(0);
    }
}

int
Broadcast(Agent *a, char *funcName, int flag)
{
    a->Req = NewRequest(a);

    if(NULL == a || NULL == a->Req) {
        return 0;
    }

    switch(flag) {
        case PDT_W_ADD_FUNC:
        {
            a->Req->dataType = PDT_W_ADD_FUNC;
            a->Req->AddFunctionPack(a->Req, funcName);
            printf("add func handle1 : %s\n", a->Req->handle);
            printf("add func handleLen1 : %d\n", a->Req->handleLen);
            break;
        }
        case PDT_W_DEL_FUNC:
        {
            a->Req->dataType = PDT_W_DEL_FUNC;
            a->Req->DelFunctionPack(a->Req, funcName);
            printf("del func handle2 : %s\n", a->Req->handle);
            printf("del func handleLen2 : %d\n", a->Req->handleLen);
            break;
        }
        default:
        {
            a->Req->dataType = PDT_W_ADD_FUNC;
            a->Req->AddFunctionPack(a->Req, funcName);
            printf("add func handle3 : %s\n", a->Req->handle);
            printf("add func handleLen3 : %d\n", a->Req->handleLen);
            break;
        }
    }

    pthread_t pid;
    int ret = pthread_create(&pid, NULL, broadcast_handler, a->Req);
    if(pthread_join(pid, NULL)) {
        perror("pthread join error\n");
        return 0;
    } else {
        printf("phtread join ok1\n");
    }
    if(ret == -1) {
        return 0;
    }

    return 1;
}

void
rw_handler(struct ev_loop *el, struct ev_io *watcher, int revents)
{
    Agent *a = watcher->data;

    if(NULL == a) {
        perror("agent error\n");
        return;
    }

    if(revents & EV_ERROR) {
        perror("event error\n");
        return;
    }

    //READ
    if(revents & EV_READ) {
        a->Res->ReadNmidRes(a->Res);

        ev_io_stop(el, &a->watcher);
        ev_io_init(&a->watcher, rw_handler, a->afd, EV_WRITE);
        ev_io_start(el, &a->watcher);
    }

    //WRITE
    if(revents & EV_WRITE) {
        a->Req->WriteNmidReq(a->Req);

        ev_io_stop(el, &a->watcher);
        ev_io_init(&a->watcher, rw_handler, a->afd, EV_READ);
        ev_io_start(el, &a->watcher);
    }

    return;
}

int
Connect(Agent *a)
{
    if(NULL == a) {
        return 0;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(a->port);
    inet_aton(a->host, &addr.sin_addr);
    //addr.sin_addr.s_addr = inet_addr("192.168.1.176");

    a->afd = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(a->afd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        return 0;
    }
    SetNonBlock(a->afd);

    pthread_mutex_lock(&(a->lock));
    a->ready = 1;
    a->w->readyNum++;
    pthread_mutex_unlock(&(a->lock));

    return 1;
}

void*
start_thread_handler(void *data)
{
    Agent *a = data;
    if(NULL == a) {
        return NULL;
    }

    pthread_mutex_lock(&(a->lock));
    a->thread_rw_id = pthread_self();
    a->running = 1;
    a->w->runningNum++;
    pthread_cond_signal(&(a->w->cond));
    pthread_mutex_unlock(&(a->lock));

    a->loop = ev_loop_new(0);
    a->watcher.data = a;
    ev_io_init(&a->watcher, rw_handler, a->afd, EV_READ);
    ev_io_start (a->loop, &a->watcher);
    ev_run(a->loop, 0);

    return NULL;
}

int
Start(Agent *a)
{
    if(NULL == a) {
        return 0;
    }

    a->Res = NewResponse(a);
    a->Req = NewRequest(a);

    pthread_t pid;
    int ret = pthread_create(&pid, NULL, start_thread_handler, a);
    if(pthread_join(pid, NULL)) {
        perror("pthread join error\n");
        return 0;
    } else {
        printf("phtread join ok2\n");
    }
    if(ret == -1) {
        Close(a);
        return 0;
    }

    return 1;
}

void
Close(Agent *a)
{
    if(NULL == a || NULL == a->w) {
        return;
    }

    ev_io_stop(a->loop, &(a->watcher));
    close(a->afd);
    a->loop = NULL;
    pthread_cancel(a->thread_rw_id);
    DelAgent(a->w->agentArr, a);
}