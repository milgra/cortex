/* Message bus */

#ifndef bus_h
#define bus_h

#include <stdio.h>

void bus_init(void);
void bus_free(void);
void bus_notify(const char* address, const char* name, void* data);
void bus_subscribe(const char* address, void (*onmessage)(const char*, void*));
void bus_unsubscribe(const char* address, void (*onmessage)(const char*, void*));

#endif

#if __INCLUDE_LEVEL__ == 0

#include "zc_map.c"
#include "zc_memory.c"
#include "zc_vector.c"

typedef struct _bus_fp
{
    void (*onmessage)(const char*, void*);
} bus_fp;

map_t* bus_subscribers = NULL;

void bus_init()
{
    if (bus_subscribers == NULL)
    {
	bus_subscribers = MNEW();
    }
}

void bus_free()
{
    REL(bus_subscribers);
    bus_subscribers = NULL;
}

// kene immediate option, vagy runloop elejen, threadsafeness?

void bus_notify(const char* source, const char* name, void* data)
{
    vec_t* listeners = MGET(bus_subscribers, source);
    if (listeners != NULL)
    {
	for (int index = 0; index < listeners->length; index++)
	{
	    bus_fp* fp = listeners->data[index];
	    fp->onmessage(name, data);
	}
    }
}

void bus_subscribe(const char* source, void (*onmessage)(const char*, void*))
{
    vec_t* listeners = MGET(bus_subscribers, source);
    if (listeners == NULL)
    {
	listeners = VNEW();
	MPUT(bus_subscribers, source, listeners);
    }

    bus_fp* data    = CAL(sizeof(bus_fp), NULL, NULL);
    data->onmessage = onmessage;
    VADD(listeners, data);
}

void bus_unsubscribe(const char* address, void (*onmessage)(const char*, void*))
{
    vec_t* listeners = map_get(bus_subscribers, address);
    if (listeners != NULL)
    {
	VREM(listeners, onmessage);
    }
}

#endif
