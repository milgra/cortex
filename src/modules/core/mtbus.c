#ifndef mtbus_h
#define mtbus_h

#include <stdio.h>

void mtbus_init(void);
void mtbus_free(void);
void mtbus_notify(const char* address, const char* name, void* data);
void mtbus_subscribe(const char* address, void (*onmessage)(const char*, void*));
void mtbus_unsubscribe(const char* address, void (*onmessage)(const char*, void*));

#endif

#if __INCLUDE_LEVEL__ == 0

#include "mtmap.c"
#include "mtmem.c"
#include "mtvec.c"

typedef struct _mtbus_fp
{
    void (*onmessage)(const char*, void*);
} mtbus_fp;

mtmap_t* mtbus_subscribers = NULL;

void mtbus_init()
{
    if (mtbus_subscribers == NULL)
    {
	mtbus_subscribers = mtmap_alloc();
    }
}

void mtbus_free()
{
    mtmem_release(mtbus_subscribers);
    mtbus_subscribers = NULL;
}

// kene immediate option, vagy runloop elejen, threadsafeness?

void mtbus_notify(const char* source, const char* name, void* data)
{
    mtvec_t* listeners = mtmap_get(mtbus_subscribers, source);
    if (listeners != NULL)
    {
	for (int index = 0; index < listeners->length; index++)
	{
	    mtbus_fp* fp = listeners->data[index];
	    fp->onmessage(name, data);
	}
    }
}

void mtbus_subscribe(const char* source, void (*onmessage)(const char*, void*))
{
    mtvec_t* listeners = mtmap_get(mtbus_subscribers, source);
    if (listeners == NULL)
    {
	listeners = mtvec_alloc();
	mtmap_put(mtbus_subscribers, source, listeners);
    }

    mtbus_fp* data  = mtmem_alloc(sizeof(mtbus_fp), NULL);
    data->onmessage = onmessage;
    mtvec_add(listeners, data);
}

void mtbus_unsubscribe(const char* address, void (*onmessage)(const char*, void*))
{
    mtvec_t* listeners = mtmap_get(mtbus_subscribers, address);
    if (listeners != NULL)
    {
	mtvec_remove(listeners, onmessage);
    }
}

#endif
