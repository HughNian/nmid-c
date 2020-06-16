//
// Created by ADMIN on 2020/4/2.
//

#include "array.h"

array *
array_new() {
    return array_new_size(ARRAY_SIZE);
}

array *
array_new_size(int size) {
    array *arr = make_new(array, 1);
    if(NULL == arr) {
        return NULL;
    }

    pthread_mutex_init(&arr->lock, NULL);
    arr->data = make_new1(pointer, size);
    arr->size = size;
    arr->cur = 0;

    return arr;
}

pointer
array_get(array *arr) {
    assert(arr);
    if(NULL == arr) {
        return NULL;
    }
    if(arr->size == 0 || arr->cur == 0) {
        return NULL;
    }

    pointer data = NULL;
    pthread_mutex_lock(&arr->lock);
    if(arr->cur > 0) {
        data = arr->data[--arr->cur];
    }
    pthread_mutex_unlock(&arr->lock);

    return data;
}

pointer
array_index(array *arr, int index) {
    assert(arr);
    assert(index >= 0);

    pointer data = NULL;

    pthread_mutex_lock(&arr->lock);
    if(arr->cur >= index) {
        data = arr->data[index];
    }
    pthread_mutex_unlock(&arr->lock);

    return data;
}

int
array_add(array *arr, pointer data) {
    assert(arr);
    assert(data);
    pthread_mutex_lock(&arr->lock);
    if(arr->cur < arr->size) {
        arr->data[arr->cur++] = data;
    } else {
        size_t newsize = arr->size * 2;
        pointer newdata = make_realloc(arr->data, sizeof(pointer) * newsize);
        if(newdata) {
            arr->data = newdata;
            arr->size = newsize;
            arr->data[arr->cur++] = data;
        } else {
            pthread_mutex_unlock(&arr->lock);
            return 0;
        }
    }
    pthread_mutex_unlock(&arr->lock);

    return 1;
}