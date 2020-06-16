#ifndef LIBNMID_WORKER_WORKER_H
#define LIBNMID_WORKER_WORKER_H

#include "common.h"

struct _Worker {
    pthread_cond_t cond;
    pthread_mutex_t lock;

    uint     arrNum;
    AgentArr *agentArr;
    uint     funcNum;
    FuncMap  *funcs;

    uint   readyNum;
    uint   runningNum;
};

Worker *NewWorker(void);

int AddServer(Worker *w, char *host, int port);

int AddFunction(Worker *w, char *funcName, JobFunc jfunc);

int DelFunction(Worker *w, char *funcName);

int Ready(Worker *w);

int Work(Worker *w);

#endif //LIBNMID_WORKER_WORKER_H
