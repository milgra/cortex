#ifndef settings_h
#define settings_h

#include "mtfile.c"
#include "zc_map.c"
#include "zc_string.c"

typedef struct _settings_t settings_t;
struct _settings_t
{
    map_t* map;
    char*  path;
    char   changed;
};

void              settings_init(const char* path, char* name);
void              settings_free(void);
void              settings_reset(void);
void              settings_del(const char* key);
void              settings_set(const char* key, type_container_t* value);
void              settings_setint(const char* key, int value);
void              settings_setfloat(const char* key, float value);
void              settings_setstring(const char* key, str_t* value);
void              settings_setunsigned(const char* key, uint32_t value);
type_container_t* settings_get(const char* key);
int               settings_getint(const char* key);
float             settings_getfloat(const char* key);
str_t*            settings_getstring(const char* key);
uint32_t          settings_getunsigned(const char* key);

#endif

#if __INCLUDE_LEVEL__ == 0

#include "linux/limits.h"
#include "zc_cstring.c"
#include "zc_memory.c"

/* creates settings */

settings_t settings;

void settings_init(const char* path, char* name)
{
    settings.path    = cstr_new_format(PATH_MAX, "%s%s", path, name, NULL);
    settings.map     = mtfile_readfile(settings.path);
    settings.changed = 0;

    if (settings.map == NULL)
    {
	settings.map = MNEW();
	mtfile_writetofile(settings.map, settings.path);
    }
}

/* deletes settings */

void settings_free()
{
    REL(settings.map);
    REL(settings.path);
}

/* resets settings */

void settings_reset()
{
    REL(settings.map);
    settings.map = MNEW();
    remove(settings.path);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* deletes value and key */

void settings_del(const char* key)
{
    map_del(settings.map, key);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* sets type container for given key */

void settings_set(const char* key, type_container_t* value)
{
    map_put(settings.map, key, value);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* sets integer for given key */

void settings_setint(const char* key, int value)
{
    char numstring[64] = {0};
    snprintf(numstring, 64, "%i", value);
    str_t*            string    = str_frombytes(numstring);
    type_container_t* container = mtfile_defaultcontainer(kTypeString, string);
    map_put(settings.map, key, container);
    REL(string);
    REL(container);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* sets float for given key */

void settings_setfloat(const char* key, float value)
{
    char numstring[64] = {0};
    snprintf(numstring, 64, "%.4f", value);
    str_t*            string    = str_frombytes(numstring);
    type_container_t* container = mtfile_defaultcontainer(kTypeString, string);
    map_put(settings.map, key, container);
    REL(string);
    REL(container);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* sets string for given key */

void settings_setstring(const char* key, str_t* value)
{
    type_container_t* container = mtfile_defaultcontainer(kTypeString, value);
    map_put(settings.map, key, container);
    REL(container);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* sets unsigned int for given key */

void settings_setunsigned(const char* key, uint32_t value)
{
    char numstring[64] = {0};
    snprintf(numstring, 64, "%lu", (unsigned long) value);
    str_t*            string    = str_frombytes(numstring);
    type_container_t* container = mtfile_defaultcontainer(kTypeString, string);
    map_put(settings.map, key, container);
    REL(string);
    REL(container);
    mtfile_writetofile(settings.map, settings.path);
    settings.changed = 1;
}

/* returns type container for given key */

type_container_t* settings_get(const char* key)
{
    type_container_t* result = map_get(settings.map, key);
    return result;
}

/* returns integer for given key */

int settings_getint(const char* key)
{
    type_container_t* value = map_get(settings.map, key);
    if (value != NULL) return str_intvalue(value->data);
    else return 0;
}

/* returns float for given key */

float settings_getfloat(const char* key)
{
    type_container_t* value = map_get(settings.map, key);
    if (value != NULL) return str_floatvalue(value->data);
    else return 0.0;
}

/* returns string for given key */

str_t* settings_getstring(const char* key)
{
    type_container_t* value = map_get(settings.map, key);
    if (value != NULL) return value->data;
    else return NULL;
}

/* returns unsigned int for given key */

uint32_t settings_getunsigned(const char* key)
{
    type_container_t* value = map_get(settings.map, key);
    if (value != NULL) return (uint32_t) str_unsignedvalue(value->data);
    else return 0;
}

#endif
