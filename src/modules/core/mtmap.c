#ifndef mtmap_h
#define mtmap_h

#include "mtvec.c"
#include <stdio.h>
#include <stdlib.h>

typedef struct pair_t pair_t;
struct pair_t
{
    char* key;
    void* value;
};

typedef struct bucket_t bucket_t;
struct bucket_t
{
    unsigned int count;
    pair_t*      pairs;
};

typedef struct mtmap_t mtmap_t;
struct mtmap_t
{
    unsigned int count_real;
    unsigned int count;
    bucket_t*    buckets;
};

mtmap_t* mtmap_alloc(void);
void     mtmap_dealloc(void* pointer);
void     mtmap_reset(mtmap_t* map);

int   mtmap_put(mtmap_t* map, const char* key, void* value);
void* mtmap_get(mtmap_t* map, const char* key);
void  mtmap_del(mtmap_t* map, const char* key);

mtvec_t* mtmap_keys(mtmap_t* map);
mtvec_t* mtmap_values(mtmap_t* map);
void     mtmap_printkeys(mtmap_t* map);

#ifdef DEBUG
void mtmap_test(void);
#endif

#endif

#if __INCLUDE_LEVEL__ == 0

#include "mtmem.c"
#include <string.h>

/* creates map */

mtmap_t* mtmap_alloc()
{
    mtmap_t* map = mtmem_calloc(sizeof(mtmap_t), mtmap_dealloc);

    if (map == NULL) return NULL;

    map->count_real = 10;
    map->count      = 0;
    map->buckets    = mtmem_calloc(map->count_real * sizeof(bucket_t), NULL);

    if (map->buckets == NULL)
    {
	mtmem_release(map);
	return NULL;
    }

    return map;
}

/* deletes map */

void mtmap_dealloc(void* pointer)
{
    mtmap_t* map = pointer;

    unsigned int index, bindex;
    bucket_t*    bucket;
    pair_t*      pair;
    pair_t*      last;

    bucket = map->buckets;
    index  = 0;

    // clean up buckets and pairs

    while (index < map->count_real)
    {
	last = pair = bucket->pairs;
	if (pair != NULL)
	{
	    bindex = 0;
	    while (bindex < bucket->count)
	    {
		mtmem_release(pair->key);
		mtmem_release(pair->value);
		pair++;
		bindex++;
	    }
	}
	if (last != NULL) mtmem_release(last);
	bucket++;
	index++;
    }

    mtmem_release(map->buckets);
}

/* resets map */

void mtmap_reset(mtmap_t* map)
{
    mtmap_dealloc(map);

    map->count_real = 10;
    map->count      = 0;
    map->buckets    = mtmem_calloc(map->count_real * sizeof(bucket_t), NULL);
}

/* resizes map */

void mtmap_resize(mtmap_t* map)
{
    // create new map

    mtmap_t* newmap    = mtmem_calloc(sizeof(mtmap_t), mtmap_dealloc);
    newmap->count_real = map->count_real * 2;
    newmap->count      = 0;
    newmap->buckets    = mtmem_calloc(newmap->count_real * sizeof(bucket_t), NULL);

    // put old values in new map

    mtvec_t* oldkeys = mtmap_keys(map);
    for (uint32_t index = 0; index < oldkeys->length; index++)
    {
	char* key   = oldkeys->data[index];
	void* value = mtmap_get(map, key);
	mtmap_put(newmap, key, value);
    }
    mtmem_release(oldkeys);

    // dealloc old map

    mtmap_dealloc(map);

    map->count_real = newmap->count_real;
    map->buckets    = newmap->buckets;
    map->count      = newmap->count;

    unsigned char* bytes = (unsigned char*) newmap;
    bytes -= sizeof(struct mtmem_head);
    free(bytes);
}

/* returns corresponding pair from bucket */

static pair_t* get_pair(bucket_t* bucket, const char* key)
{
    unsigned int index;
    pair_t*      pair;

    if (bucket->count == 0) return NULL;

    pair  = bucket->pairs;
    index = 0;

    while (index < bucket->count)
    {
	if (pair->key != NULL && pair->value != NULL)
	{
	    if (strcmp(pair->key, key) == 0) return pair;
	}
	pair++;
	index++;
    }

    return NULL;
}

/* returns a hash code for the provided string */

static unsigned long hash(const char* str)
{
    unsigned long hash = 5381;
    int           chr;
    while ((chr = *str++)) hash = ((hash << 5) + hash) + chr;
    return hash;
}

/* puts in value with key */

int mtmap_put(mtmap_t* map, const char* key, void* value)
{
    mtmem_retain(value);

    size_t    index;
    bucket_t* bucket;
    pair_t *  tmp_pairs, *pair;

    if (map == NULL) return 0;
    if (key == NULL) return 0;

    // get a pointer to the bucket the key string hashes to

    index  = hash(key) % map->count_real;
    bucket = &(map->buckets[index]);

    // check if we can handle insertion by simply replacing
    // an existing value in a key-value pair in the bucket.

    if ((pair = get_pair(bucket, key)) != NULL)
    {
	// the bucket contains a pair that matches the provided key,
	// change the value for that pair to the new value.

	mtmem_release(pair->value);
	pair->value = value;
	return 1;
    }

    // create a key-value pair

    if (bucket->count == 0)
    {
	// the bucket is empty, lazily allocate space for a single
	// key-value pair.

	bucket->pairs = mtmem_calloc(sizeof(pair_t), NULL);
	if (bucket->pairs == NULL) return 0;
	bucket->count = 1;
    }
    else
    {
	// the bucket wasn't empty but no pair existed that matches the provided
	// key, so create a new key-value pair.

	tmp_pairs = mtmem_realloc(bucket->pairs, (bucket->count + 1) * sizeof(pair_t));
	if (tmp_pairs == NULL) return 0;
	bucket->pairs = tmp_pairs;
	bucket->count++;
    }

    // get the last pair in the chain for the bucket

    pair        = &(bucket->pairs[bucket->count - 1]);
    pair->key   = mtmem_calloc((strlen(key) + 1) * sizeof(char), NULL);
    pair->value = value;

    map->count += 1;

    // copy the key

    strcpy(pair->key, key);

    if (map->count == map->count_real) mtmap_resize(map);

    return 1;
}

/* returns value for key */

void* mtmap_get(mtmap_t* map, const char* key)
{
    unsigned int index;
    bucket_t*    bucket;
    pair_t*      pair;

    if (map == NULL) return NULL;
    if (key == NULL) return NULL;

    index  = hash(key) % map->count_real;
    bucket = &(map->buckets[index]);

    pair = get_pair(bucket, key);
    if (pair == NULL) return NULL;
    return pair->value;
}

/* removes value for key */

void mtmap_del(mtmap_t* map, const char* key)
{
    unsigned int index, found = 0;
    bucket_t*    bucket;
    pair_t*      pair;

    index  = hash(key) % map->count_real;
    bucket = &(map->buckets[index]);

    if (bucket->count > 0)
    {
	pair  = bucket->pairs;
	index = 0;
	while (index < bucket->count)
	{
	    if (pair->key != NULL && pair->value != NULL)
	    {
		if (strcmp(pair->key, key) == 0)
		{
		    found = 1;
		    break;
		}
	    }
	    pair++;
	    index++;
	}

	if (found == 1)
	{
	    mtmem_release(pair->key);
	    mtmem_release(pair->value);

	    pair = bucket->pairs;
	    if (index < bucket->count)
	    {
		memmove(pair + index, pair + index + 1, (bucket->count - index - 1) * sizeof(bucket_t));
	    }
	    bucket->count -= 1;

	    if (bucket->count == 0)
	    {
		mtmem_release(bucket->pairs);
		bucket->pairs = NULL;
	    }
	    map->count -= 1;
	}
    }
}

/* returns all keys in map */

mtvec_t* mtmap_keys(mtmap_t* map)
{
    mtvec_t* result = mtvec_alloc();

    unsigned int index, bindex;
    bucket_t*    bucket;
    pair_t*      pair;

    if (map == NULL) return NULL;
    bucket = map->buckets;
    index  = 0;
    while (index < map->count_real)
    {
	pair   = bucket->pairs;
	bindex = 0;
	while (bindex < bucket->count)
	{
	    mtvec_add(result, pair->key);
	    pair++;
	    bindex++;
	}
	bucket++;
	index++;
    }
    return result;
}

/* returns all values in map */

mtvec_t* mtmap_values(mtmap_t* map)
{
    mtvec_t* result = mtvec_alloc();

    unsigned int index, bindex;
    bucket_t*    bucket;
    pair_t*      pair;

    if (map == NULL) return NULL;
    bucket = map->buckets;
    index  = 0;
    while (index < map->count_real)
    {
	pair   = bucket->pairs;
	bindex = 0;
	while (bindex < bucket->count)
	{
	    mtvec_add(result, pair->value);
	    pair++;
	    bindex++;
	}
	bucket++;
	index++;
    }
    return result;
}

/* prints keys */

void mtmap_printkeys(mtmap_t* map)
{
    mtvec_t* keys = mtmap_keys(map);
    printf(" \n");
    for (int index = 0; index < keys->length; index++) printf(" %s", keys->data[index]);
}

// tests map

#ifdef DEBUG
void mtmap_test()
{
    printf("MAP TEST SESSION START");
    printf("1 CREATE EMPTY");
    mtmap_t* m1 = mtmap_alloc();
    printf("2 DELETE EMPTY");
    mtmem_release(m1);
    printf("3 ADDING DATA");
    mtmap_t* m2 = mtmap_alloc();
    mtmap_put(m2, "fakk", "fakkvalue");
    mtmap_put(m2, "makk", "makkvalue");
    mtmap_put(m2, "takk", "takkvalue");
    mtmap_put(m2, "kakk", "kakkvalue");
    printf("4 GETTING DATA");
    printf(" VALUE FOR makk : %s", mtmap_get(m2, "makk"));
    printf("5 SETTING DATA TO NULL");
    mtmap_put(m2, "takk", NULL);
    printf(" VALUE FOR takk : %s", mtmap_get(m2, "takk"));
    printf("MAP TEST SESSION END");
}
#endif

#endif
