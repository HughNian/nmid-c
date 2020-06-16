#ifndef HASH_H
#define HASH_H

#include "list.h"

typedef unsigned long int ub4;
typedef unsigned char ub1;

typedef struct _sHashNode  HashNode;
typedef struct _sHashTable sHashTable;
typedef struct _sHashKey   HashKey;

struct _sHashNode {
	struct list_head list;
	HashNode	*next;
	void		*value;
	ub4			h;
	int			keyLength;
	char		key[1];
};

struct _sHashTable {
	unsigned int size;
	unsigned int used;
	HashNode **bucket;
	struct list_head list;
};

struct _sHashKey {
	char *key;
	int keyLength;
};

typedef void (*hash_destroy_function)(void *);
typedef int (*hash_foreach_handler)(char *key, int keyLength, void *value);

sHashTable *hash_alloc(int size);
int hash_insert(sHashTable *htb, char *key, void *value);
int hash_insert_bykey(sHashTable *htb, HashKey *key, void *value);
int hash_lookup(sHashTable *htb, char *key, void **retval);
int hash_replace(sHashTable *htb, char *key, void *nvalue, void **retval);
int hash_remove(sHashTable *htb, char *key, void **retval);
void hash_destroy(sHashTable *htb, hash_destroy_function destroy);
void hash_try_resize(sHashTable *htb);
int hash_foreach(sHashTable *htb, hash_foreach_handler handler);

#endif
