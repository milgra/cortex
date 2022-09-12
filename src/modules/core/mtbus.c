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

#include "zc_map.c"
#include "zc_memory.c"
#include "zc_vector.c"

typedef struct _mtbus_fp
{
    void (*onmessage)(const char*, void*);
} mtbus_fp;

map_t* mtbus_subscribers = NULL;

void mtbus_init()
{
    if (mtbus_subscribers == NULL)
    {
	mtbus_subscribers = MNEW();
    }
}

void mtbus_free()
{
    REL(mtbus_subscribers);
    mtbus_subscribers = NULL;
}

// kene immediate option, vagy runloop elejen, threadsafeness?

void mtbus_notify(const char* source, const char* name, void* data)
{
    vec_t* listeners = MGET(mtbus_subscribers, source);
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
    vec_t* listeners = MGET(mtbus_subscribers, source);
    if (listeners == NULL)
    {
	listeners = VNEW();
	MPUT(mtbus_subscribers, source, listeners);
    }

    mtbus_fp* data  = CAL(sizeof(mtbus_fp), NULL, NULL);
    data->onmessage = onmessage;
    VADD(listeners, data);
}

void mtbus_unsubscribe(const char* address, void (*onmessage)(const char*, void*))
{
    vec_t* listeners = map_get(mtbus_subscribers, address);
    if (listeners != NULL)
    {
	VREM(listeners, onmessage);
    }
}

#endif
