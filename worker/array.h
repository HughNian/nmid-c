//
// Created by ADMIN on 2020/4/2.
//

#ifndef LIBNMID_WORKER_ARRAY_H
#define LIBNMID_WORKER_ARRAY_H

#include "common.h"

struct _array {
    pthread_mutex_t lock;
    pointer* data;
    int size;
    int cur;
};

typedef struct _array array;

array *array_new();

array *array_new_size(int size);

pointer array_get(array *arr);

pointer array_index(array *arr, int index);

int array_add(array *arr, pointer data);

#endif //LIBNMID_WORKER_ARRAY_H
