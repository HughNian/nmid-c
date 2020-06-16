#include "function.h"

uint
Hash(const char *s, int len)
{
    uint h = 0;
    int  i = 0;
    assert(s);

    if(len < 0) {
        len = (s ? (int)strlen(s) : 0);
    }

    while(i++ < len) {
        h = 17*h + *s++;
    }

    return h;
}

static inline uint
get_index(uint length, uint hashvalue)
{
    return (hashvalue % length);
};

FuncMap *
NewFuncMap(int size)
{
    if(size <= 0) {
        size = MAP_SIZE;
    }

    FuncMap *fmap = make_new(FuncMap, 1);
    if(fmap == NULL) {
        return NULL;
    }
    memset(fmap, 0, sizeof(FuncMap));
    pthread_mutex_init(&fmap->lock, NULL);
    fmap->size = (uint)size;
    fmap->length = 0;

    Func **fs;
    fs = make_new(Func*, size);
    if(NULL == fs) {
        make_free(fmap);
        return NULL;
    }
    memset(fs, 0, sizeof(Func*)*size);
    fmap->functions = fs;

    uint *keys = make_new(uint, size);
    if(NULL == keys) {
        make_free(fs);
        make_free(fmap);
        return NULL;
    }
    fmap->keys = keys;

    return fmap;
}

int
reNewFuncMap(FuncMap *fmap)
{
    if(NULL == fmap) {
        return 0;
    }
    if(fmap->length == 0 || fmap->size == 0) {
        return 0;
    }

    pthread_mutex_lock(&fmap->lock);
    int newSize;
    Func **newFunc;
    fmap->size *= 2;
    newSize = sizeof(Func *) * fmap->size;
    newFunc = (Func **)make_realloc(fmap->functions, newSize);
    if(NULL == newFunc) {
        pthread_mutex_unlock(&fmap->lock);
        return 0;
    }
    uint *newKeys = (uint *)make_realloc(fmap->keys, newSize);
    if(NULL == newKeys) {
        pthread_mutex_unlock(&fmap->lock);
        make_free(fmap->functions);
        return 0;
    }

    fmap->functions = newFunc;
    fmap->keys = newKeys;
    pthread_mutex_unlock(&fmap->lock);

    return 1;
}

Func *
NewFunc(char *funcName, JobFunc job)
{
    Func *f = make_new(Func, 1);
    if(f == NULL) {
        return NULL;
    }
    f->hashval = Hash(funcName, strlen(funcName));
    f->func = job;
    f->funcName = funcName;

    return f;
}

int
AddFunc(FuncMap *fmap, Func *f)
{
    if(NULL == fmap || NULL == f) {
        return 0;
    }

    if(fmap->length >= fmap->size) {
        reNewFuncMap(fmap);
    }

    pthread_mutex_lock(&fmap->lock);
    uint key = get_index(fmap->size, f->hashval);
    f->key = key;
    fmap->functions[f->key] = f;
    fmap->keys[fmap->length++] = f->key;
    pthread_mutex_unlock(&fmap->lock);

    return 1;
}

int
DelFunc(FuncMap *fmap, Func *f)
{
    if(NULL == fmap || NULL == f) {
        return 0;
    }

    int i = 0;
    int lkey = 0;
    int len = fmap->length;
    for(; i < len; i++) {
        if(f->key == fmap->keys[i]) {
            lkey = i;
        }
    }

    uint *newKeys = make_new(uint, len-1);
    i = 0;
    for(; i < len; i++) {
        if(i < lkey) {
            newKeys[i] = fmap->keys[i];
        } else {
            newKeys[i] = fmap->keys[i+1];
        }
    }

    pthread_mutex_lock(&fmap->lock);
    fmap->length--;
    pthread_mutex_unlock(&fmap->lock);

    Func **newFs;
    newFs = make_new(Func*, fmap->size);
    memset(newFs, 0, sizeof(Func*)*fmap->size);
    i = 0;
    for(; i < fmap->length; i++) {
        newFs[newKeys[i]] = fmap->functions[newKeys[i]];
    }
    make_free(f);
    make_free(fmap->functions);
    fmap->keys = newKeys;
    fmap->functions = newFs;

    return 1;
}

Func *
GetFunc(FuncMap *fmap, char *funcName)
{
    if(NULL == fmap) {
        return NULL;
    }
    if(fmap->size == 0 || fmap->length == 0) {
        return NULL;
    }
    if(strlen(funcName) == 0) {
        return NULL;
    }

    pthread_mutex_lock(&fmap->lock);
    uint hashval, key;
    Func *function;
    hashval = Hash(funcName, strlen(funcName));
    key = get_index(fmap->size, hashval);
    function = fmap->functions[key];
    if(NULL == function) {
        pthread_mutex_unlock(&fmap->lock);
        return NULL;
    }
    pthread_mutex_unlock(&fmap->lock);

    return function;
}