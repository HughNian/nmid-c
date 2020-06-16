#ifndef LIBNMID_WORKER_AGENT_H
#define LIBNMID_WORKER_AGENT_H

#include "common.h"

struct _Agent {
    pthread_cond_t cond;
    pthread_mutex_t lock;

    pthread_t thread_rw_id;

    int curIndex;

    struct ev_loop *loop;
    struct ev_io watcher;

    int  afd;
    char *host;
    int  port;
    uint32_t ready;
    uint32_t running;

    Request  *Req;
    Response *Res;

    Worker *w;
};

struct _AgentArr {
    pthread_cond_t cond;
    pthread_mutex_t lock;

    int size;
    int cur;
    Agent **a;
};

AgentArr *NewAgentArr(int size);

Agent *NewAgent(Worker *w, char *host, int port);

int AddAgent(AgentArr *agentArr, Agent *a);

int DelAgent(AgentArr *agentArr, Agent *a);

int Grab(Agent *a);

int Broadcast(Agent *a, char *funcName, int flag);

int Connect(Agent *a);

int Start(Agent *a);

void Close(Agent *a);

#endif //LIBNMID_WORKER_AGENT_H
