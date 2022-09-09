#ifndef settings_h
#define settings_h

#include "mtfile.c"
#include "mtmap.c"
#include "mtstr.c"

typedef struct _settings_t settings_t;
struct _settings_t
{
    mtmap_t* map;
    char*    path;
    char     changed;
};

void              settings_init(const char* path, char* name);
void              settings_free(void);
void              settings_reset(void);
void              settings_del(const char* key);
void              settings_set(const char* key, type_container_t* value);
void              settings_setint(const char* key, int value);
void              settings_setfloat(const char* key, float value);
void              settings_setstring(const char* key, mtstr_t* value);
void              settings_setunsigned(const char* key, uint32_t value);
type_container_t* settings_get(const char* key);
int               settings_getint(const char* key);
float             settings_getfloat(const char* key);
mtstr_t*          settings_getstring(const char* key);
uint32_t          settings_getunsigned(const char* key);

#endif

#if __INCLUDE_LEVEL__ == 0

#include "mtcstr.c"
#include "mtmem.c"

/* creates settings */

settings_t settings;

void settings_init(const char* path, char* name)
{
    settings.path    = mtcstr_fromformat("%s%s", path, name, NULL);
    settings.map     = mtfile_readfile(settings.path);
    settings.changed = 0;

    if (settings.map == NULL)
    {
	settings.map = mtmap_alloc();
	mtfile_writetofile(settings.map, settings.path);
    }
}

/* deletes settings */

void settings_free()
{
    mtmem_release(settings.map);
    mtmem_release(settings.path);
}

/* resets settings */

void settings_reset()
{
    mtmem_release(settings.map);
    settings.map = mtmap_alloc();
    remove(settings.path);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* deletes value and key */

void settings_del(const char* key)
{
    mtmap_del(settings.map, key);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* sets type container for given key */

void settings_set(const char* key, type_container_t* value)
{
    mtmap_put(settings.map, key, value);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* sets integer for given key */

void settings_setint(const char* key, int value)
{
    char numstring[64] = {0};
    snprintf(numstring, 64, "%i", value);
    mtstr_t*          string    = mtstr_frombytes(numstring);
    type_container_t* container = mtfile_defaultcontainer(kTypeString, string);
    mtmap_put(settings.map, key, container);
    mtmem_release(string);
    mtmem_release(container);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* sets float for given key */

void settings_setfloat(const char* key, float value)
{
    char numstring[64] = {0};
    snprintf(numstring, 64, "%.4f", value);
    mtstr_t*          string    = mtstr_frombytes(numstring);
    type_container_t* container = mtfile_defaultcontainer(kTypeString, string);
    mtmap_put(settings.map, key, container);
    mtmem_release(string);
    mtmem_release(container);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* sets string for given key */

void settings_setstring(const char* key, mtstr_t* value)
{
    type_container_t* container = mtfile_defaultcontainer(kTypeString, value);
    mtmap_put(settings.map, key, container);
    mtmem_release(container);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* sets unsigned int for given key */

void settings_setunsigned(const char* key, uint32_t value)
{
    char numstring[64] = {0};
    snprintf(numstring, 64, "%lu", (unsigned long) value);
    mtstr_t*          string    = mtstr_frombytes(numstring);
    type_container_t* container = mtfile_defaultcontainer(kTypeString, string);
    mtmap_put(settings.map, key, container);
    mtmem_release(string);
    mtmem_release(container);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* returns type container for given key */

type_container_t* settings_get(const char* key)
{
    type_container_t* result = mtmap_get(settings.map, key);
    return result;
}

/* returns integer for given key */

int settings_getint(const char* key)
{
    type_container_t* value = mtmap_get(settings.map, key);
    if (value != NULL) return mtstr_intvalue(value->data);
    else return 0;
}

/* returns float for given key */

float settings_getfloat(const char* key)
{
    type_container_t* value = mtmap_get(settings.map, key);
    if (value != NULL) return mtstr_floatvalue(value->data);
    else return 0.0;
}

/* returns string for given key */

mtstr_t* settings_getstring(const char* key)
{
    type_container_t* value = mtmap_get(settings.map, key);
    if (value != NULL) return value->data;
    else return NULL;
}

/* returns unsigned int for given key */

uint32_t settings_getunsigned(const char* key)
{
    type_container_t* value = mtmap_get(settings.map, key);
    if (value != NULL) return (uint32_t) mtstr_unsignedvalue(value->data);
    else return 0;
}

#endif
