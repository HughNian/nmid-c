#ifndef LIBNMID_GO_WORKER_FUNCTION_H
#define LIBNMID_GO_WORKER_FUNCTION_H

#include "common.h"

struct _Func {
    uint hashval;
    uint key;
    JobFunc func;
    char *funcName;
};

struct _FuncMap {
    pthread_mutex_t lock;
    uint  size;
    uint  length;
    Func **functions;
    uint *keys;
};

FuncMap *NewFuncMap(int size);

Func *NewFunc(char *funcName, JobFunc job);

int AddFunc(FuncMap *fmap, Func *f);

int DelFunc(FuncMap *fmap, Func *f);

Func *GetFunc(FuncMap *fmap, char *funcName);

uint Hash(const char *s, int len);

#endif //LIBNMID_GO_WORKER_FUNCTION_H
