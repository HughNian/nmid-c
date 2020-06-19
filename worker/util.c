#include "util.h"
#include "rw.h"

void
SetNonBlock(int fd)
{
    int flags;
    flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
}

char *
StrtoUpper(char *str)
{
    if(str == NULL) return NULL;

    int len = strlen(str);
    char *p;
    p = (char *)malloc(len+1);
    int i = 0;
    while(*str && i != len) {
        *(p+i) = toupper(*str);
        str++;
        i++;
    }
    *(p+i) = '\0';

    return p;
}

char *
LeftSubStr(char *dst,char *src, int n)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len;
    /*p += (len-n);*/   /*从右边第n个字符开始*/
    while(n--) *(q++) = *(p++);
    *(q++)='\0';

    return dst;
}


char *
MidSubStr(char *dst,char *src, int n, int m) /*n为长度，m为位置*/
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len-m;    /*从第m个到最后*/
    if(m<0) m=0;    /*从第一个开始*/
    if(m>len) return NULL;
    p += m;
    while(n--) *(q++) = *(p++);
    *(q++)='\0';

    return dst;
}

char *
RightSubStr(char *dst,char *src, int n)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len;
    p += (len-n);
    while(*(q++) = *(p++));

    return dst;
}

char *
SubStrLastVal(char *last)
{
    assert(last);

    char *val;

    if(strstr(last, "]")) {
        int len = strlen(last)-2;
        if(len < 2) {
            val = strdup(last);
            return val;
        }

        val = (char *)malloc(len+1);
        memset(val, 0, len+1);

        strncpy(val, last, len);
        val[len] = '\0';
        return val;
    } else {
        val = strdup(last);
        return val;
    }
}

char *
ReqMsgPackEncode(RetStruct *ret, size_t *ret_size, msgpack_sbuffer *sbuf)
{
    assert(ret);

    msgpack_packer pk;
    msgpack_sbuffer_init(sbuf);
    msgpack_packer_init(&pk, sbuf, msgpack_sbuffer_write);

    msgpack_pack_array(&pk, 3);
    msgpack_pack_int(&pk, ret->code);
    msgpack_pack_str(&pk, strlen(ret->msg));
    msgpack_pack_str_body(&pk, ret->msg, strlen(ret->msg));
    msgpack_pack_str(&pk, strlen(ret->data));
    msgpack_pack_str_body(&pk, ret->data, strlen(ret->data));

    *ret_size = sbuf->size;

    return sbuf->data;
}

char *
ResMsgPackDecode(const char *packbuf, size_t pack_len)
{
    int i = 0;
    size_t off = 0;
    char *tmp_buf;
    char *unpack_buf = (char *)malloc(pack_len*2);

    msgpack_unpacked result;
    msgpack_unpack_return ret;
    msgpack_unpacked_init(&result);

    ret = msgpack_unpack_next(&result, packbuf, pack_len, &off);
    while(ret == MSGPACK_UNPACK_SUCCESS) {
        msgpack_object obj = result.data;

        msgpack_object_print_buffer(unpack_buf, pack_len*2, obj);
        if(strstr(unpack_buf, ":") == NULL) break;
        tmp_buf = strdup(unpack_buf);

        ret = msgpack_unpack_next(&result, packbuf, pack_len, &off);

        i++;
    }

    msgpack_unpacked_destroy(&result);
    free(unpack_buf);

    printf("tmp_buf - %s\n", tmp_buf);
    return tmp_buf;
}

int
GetParamsNum(const char *params)
{
    int i = 0;
    int num = 0;
    int len = strlen(params);

    for(; i < len; i++) {
        if(params[i] == PARAMS_SCOPE) {
            num++;
        }
    }

    return num;
}

char **
GetParamsArr(const char *params, int arr_num)
{
    assert(arr_num);

    char **Arr = make_new(char *, arr_num);
    if(NULL == Arr) {
        return NULL;
    }

    int n = 0;
    char *tmp = (char *)memchr(params, ':', strlen(params));
    while(tmp) {
        if(n >= arr_num) break;

        int i = 0;
        char *_tmp = strdup(tmp+1);
        if(arr_num > 1) {
            if(arr_num - 1 == n) {
                while(*_tmp != ']')  {
                    _tmp = _tmp + 1;
                    i++;
                }
            } else {
                while(*_tmp != ',')  {
                    _tmp = _tmp + 1;
                    i++;
                }
            }

            printf("while i - %d\n", i);
            *(tmp+i) = '\0';
        }

        printf("n - %d\n", n);

        //对最后一个值截取下
        if(arr_num - 1 == n) {
            Arr[n] = SubStrLastVal(tmp+1);
        } else {
            Arr[n] = strdup(tmp+1);
        }

        tmp = strdup(_tmp);
        tmp = (char *)memchr(tmp, ':', strlen(tmp));
        n++;
    }

    return Arr;
}

void
FreeParamsArr(char **params_arr, int arr_num)
{
    assert(params_arr);
    assert(arr_num);

    int i = 0;
    for(; i < arr_num; i++) {
        free(params_arr[i]);
    }

    make_free(params_arr);
}