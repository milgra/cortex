#ifndef mtmem_h
#define mtmem_h

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*

    There is no REL macro, RPL( x , NULL ) should be used because that sets heap value
    to NULL and helps debugging with instant segfaults.

*/

// raise stack memory block to heap memory

#define HEAP(X) mtmem_stack_to_heap(sizeof(X), NULL, (unsigned char*) &X)

// retain new value as old value

#define RPL(X, Y) mtmem_replace((void**) &X, Y)

// set new value as old value without retain count alternation

#define SET(X, Y) X = Y
#define REL(X) mtmem_release(X)

struct mtmem_head
{
    void (*destructor)(void*);
    size_t retaincount;
};

void*  mtmem_alloc(size_t size, void (*destructor)(void*));
void*  mtmem_calloc(size_t size, void (*destructor)(void*));
void*  mtmem_realloc(void* pointer, size_t size);
void*  mtmem_retain(void* pointer);
char   mtmem_release(void* pointer);
char   mtmem_releaseeach(void* first, ...);
void   mtmem_track(void* pointer, uint8_t force);
size_t mtmem_retaincount(void* pointer);
void   mtmem_replace(void** address, void* data);
void*  mtmem_stack_to_heap(size_t size, void (*destructor)(void*), unsigned char* data);

#endif

#if __INCLUDE_LEVEL__ == 0

#include <string.h>

static void* mtmem_tracked = NULL;

/* allocs memory with stored destructor and retaincount at the beginning of the section */

void* mtmem_alloc(size_t size, void (*destructor)(void*))
{
    if (size == 0) return NULL;
    unsigned char* bytes = malloc(sizeof(struct mtmem_head) + size);
    assert(bytes != NULL);

    struct mtmem_head* head = (struct mtmem_head*) bytes;

    head->destructor  = destructor;
    head->retaincount = 1;

    return bytes + sizeof(struct mtmem_head);
}

void* mtmem_stack_to_heap(size_t size, void (*destructor)(void*), unsigned char* data)
{
    unsigned char* bytes = mtmem_alloc(size, destructor);
    memcpy(bytes, data, size);
    return bytes;
}

/* callocs memory with stored destructor and retaincount at the beginning of the section */

void* mtmem_calloc(size_t size, void (*destructor)(void*))
{
    if (size == 0) return NULL;
    unsigned char* bytes = calloc(1, sizeof(struct mtmem_head) + size);
    assert(bytes != NULL);

    struct mtmem_head* head = (struct mtmem_head*) bytes;

    head->destructor  = destructor;
    head->retaincount = 1;

    return bytes + sizeof(struct mtmem_head);
}

/* reallocs memory with stored destructor and retaincount at the beginning of the section */

void* mtmem_realloc(void* pointer, size_t size)
{
    if (size == 0) return NULL;
    assert(pointer != NULL);

    unsigned char* bytes = (unsigned char*) pointer;
    bytes -= sizeof(struct mtmem_head);
    bytes = realloc(bytes, sizeof(struct mtmem_head) + size);
    assert(bytes != NULL);

    return bytes + sizeof(struct mtmem_head);
}

/* increases retaincount of section */

void* mtmem_retain(void* pointer)
{
    if (pointer == NULL) return NULL;

    unsigned char* bytes = (unsigned char*) pointer;
    bytes -= sizeof(struct mtmem_head);
    struct mtmem_head* head = (struct mtmem_head*) bytes;

    assert(head->retaincount < UINT16_MAX - 1);

    head->retaincount += 1;

    if (mtmem_tracked != NULL && mtmem_tracked == pointer)
    {
	printf("mem retain, retaincount %zx\n", head->retaincount);
    }

    return pointer;
}

/* decreases retaincount of section, calls destructor if exists, frees up section */

char mtmem_release(void* pointer)
{
    if (pointer == NULL) return 0;

    unsigned char* bytes = (unsigned char*) pointer;
    bytes -= sizeof(struct mtmem_head);
    struct mtmem_head* head = (struct mtmem_head*) bytes;

    assert(head->retaincount > 0);

    head->retaincount -= 1;

    if (mtmem_tracked != NULL && mtmem_tracked == pointer)
    {
	printf("mem release, retaincount %zx\n", head->retaincount);
    }

    if (head->retaincount == 0)
    {
	if (head->destructor != NULL) head->destructor(bytes + sizeof(struct mtmem_head));
	free(bytes);
	return 1;
    }

    return 0;
}

/* release multiple instances */

char mtmem_releaseeach(void* first, ...)
{
    va_list ap;
    void*   actual;
    char    released = 1;
    va_start(ap, first);
    for (actual = first; actual != NULL; actual = va_arg(ap, void*))
    {
	released &= mtmem_release(actual);
    }
    va_end(ap);
    return released;
}

/* returns retain count */

size_t mtmem_retaincount(void* pointer)
{
    assert(pointer != NULL);

    unsigned char* bytes = (unsigned char*) pointer;
    bytes -= sizeof(struct mtmem_head);
    struct mtmem_head* head = (struct mtmem_head*) bytes;

    return head->retaincount;
}

/* replaces old retained data with new retained with release of old data */

void mtmem_replace(void** address, void* data)
{
    mtmem_release(*address);
    mtmem_retain(data);
    *address = data;
}

/* tracks address */

void mtmem_track(void* pointer, uint8_t force)
{
    if (mtmem_tracked == NULL || force == 1)
    {
	printf("mem track address %zx\n", (size_t) pointer);
	mtmem_tracked = pointer;
    }
}

#endif
