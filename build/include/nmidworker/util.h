#ifndef LIBNMID_WORKER_UTIL_H
#define LIBNMID_WORKER_UTIL_H

#include "common.h"

#include <stddef.h>
#include <stdarg.h>

void SetNonBlock(int fd);

char *ResMsgPackDecode(const char *packbuf, size_t pack_len);

int GetParamsNum(const char *params);

char **GetParamsArr(const char *params, int arr_num);

void FreeParamsArr(char **params_arr, int arr_num);

char *LeftSubStr(char *dst,char *src, int n);

char *MidSubStr(char *dst,char *src, int n, int m);

char *RightSubStr(char *dst,char *src, int n);

char *StrtoUpper(char *str);

char *ReqMsgPackEncode(RetStruct *ret, size_t *ret_size, msgpack_sbuffer *sbuf);

#endif //LIBNMID_WORKER_UTIL_H
