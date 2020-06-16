#include "worker.h"
#include "agent.h"
#include "function.h"

Worker*
NewWorker()
{
    Worker *w = make_new(Worker, 1);
    if(w == NULL) {
        return NULL;
    }
    memset(w, 0, sizeof(Worker));

    pthread_cond_init(&w->cond, NULL);
    pthread_mutex_init(&w->lock, NULL);

    w->agentArr   = NewAgentArr(ARRAY_SIZE);
    w->funcs      = NewFuncMap(MAP_SIZE);
    w->arrNum     = 0;
    w->funcNum    = 0;
    w->readyNum   = 0;
    w->runningNum = 0;

    return w;
}

int
AddServer(Worker *w, char *host, int port)
{
    if(NULL == w || NULL == w->agentArr) {
        return 0;
    }
    if(strlen(host) == 0) {
        return 0;
    }

    Agent *a = NewAgent(w, host, port);
    if(NULL == a) {
        return 0;
    }

    int ret;
    ret = AddAgent(w->agentArr, a);
    if(ret) {
        w->arrNum++;
    }

    return ret;
}

int
AddFunction(Worker *w, char *funcName, JobFunc jfunc)
{
    if(NULL == w || NULL == w->funcs) {
        return 0;
    }
    if(strlen(funcName) == 0) {
        return 0;
    }

    Func *f;
    f = NewFunc(funcName, jfunc);
    if(NULL == f) {
        return 0;
    }

    int ret = AddFunc(w->funcs, f);
    if(ret) {
        pthread_mutex_lock(&w->lock);
        w->funcNum++;
        pthread_mutex_unlock(&w->lock);
    }

    int i = 0;
    for(; i < w->arrNum; i++) {
        Agent *a = w->agentArr->a[i];
        if(a->running) {
            Broadcast(a, funcName, PDT_W_ADD_FUNC);
        }
    }

    return ret;
}

int
DelFunction(Worker *w, char *funcName)
{
    if(NULL == w || NULL == w->funcs) {
        return 0;
    }
    if(strlen(funcName) == 0) {
        return 0;
    }

    Func *f;
    f = GetFunc(w->funcs, funcName);
    if(NULL == f) {
        return 0;
    }

    int ret = DelFunc(w->funcs, f);
    if(ret) {
        pthread_mutex_lock(&w->lock);
        w->funcNum--;
        pthread_mutex_unlock(&w->lock);
    }

    int i = 0;
    for(; i < w->arrNum; i++) {
        Agent *a = w->agentArr->a[i];
        if(a->running) {
            Broadcast(a, funcName, PDT_W_DEL_FUNC);
        }
    }

    return ret;
}

int
Ready(Worker *w)
{
    if(NULL == w) {
        return 0;
    }
    if(w->arrNum == 0 || w->funcNum == 0) {
        return 0;
    }
    if(w->agentArr->cur != w->arrNum || w->funcs->length != w->funcNum) {
        return 0;
    }

    int i = 0;
    for(; i  < w->arrNum; i++) {
        Connect(w->agentArr->a[i]);
    }

    return 1;
}

int
Work(Worker *w)
{
    if(NULL == w) {
        return 0;
    }
    if(w->arrNum == 0 || w->funcNum == 0) {
        return 0;
    }
    if(w->agentArr->cur != w->arrNum || w->funcs->length != w->funcNum) {
        return 0;
    }

    int i = 0;
    for(; i < w->arrNum; i++) {
        Agent *a;
        a = w->agentArr->a[i];
        if(NULL == a) continue;
        if(!a->ready) continue;

        int n = 0;
        for(; n < w->funcNum; n++) {
            uint key = w->funcs->keys[n];
            Func *function = w->funcs->functions[key];
            if(NULL == function) {
                perror("function err\n");
                continue;
            }
            if(NULL != function || strlen(function->funcName) != 0) {
                Broadcast(a, function->funcName, PDT_W_ADD_FUNC);
            }
        }
        Grab(a);
        Start(a);
    }

    for(;;) {
        sleep(0);
    }
}