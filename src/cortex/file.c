/* Config file handling */

#ifndef mtfile_h
#define mtfile_h

#include "str_util.c"
#include "zc_cstring.c"
#include "zc_map.c"
#include "zc_string.c"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define kTypeMap 1
#define kTypeString 3
#define kTypeVector 2

typedef struct type_container_t type_container_t;
struct type_container_t
{
    char  type;
    void* data;
};

void   mtfile_writetofile(map_t* map, char* path);
map_t* mtfile_readfile(char* path);
void   mtfile_appendstringtofile(str_t* string, FILE* file_pointer);
void   mtfile_appendvectortofile(vec_t* vector, FILE* file_pointer);
void   mtfile_appendmaptofile(map_t* map, FILE* file_pointer);
map_t* mtfile_readmap(FILE* file_pointer);
str_t* mtfile_readstring(FILE* file_pointer);
vec_t* mtfile_readvector(FILE* file_pointer);

vec_t*            mtfile_readlines(char* path);
type_container_t* mtfile_defaultcontainer(char type, void* data);

#endif

#if __INCLUDE_LEVEL__ == 0

#include "zc_memory.c"
#include <string.h>

/* appends string to opened file */

void mtfile_appendstringtofile(str_t* string, FILE* file_pointer)
{
    // write type

    size_t writeup;
    char   typebytes[1] = {kTypeString};
    writeup             = fwrite(typebytes, sizeof(uint8_t), 1, file_pointer);

    // write bytes

    char* bytes = str_new_cstring(string);
    writeup     = fwrite(bytes, sizeof(uint8_t), string->length_bytes, file_pointer);

    // write closing 0

    char zerobytes[1] = {0};
    writeup           = fwrite(zerobytes, sizeof(uint8_t), 1, file_pointer);

    // cleanup

    REL(bytes);
}

/* appends vector to opened file */

void mtfile_appendvectortofile(vec_t* vector, FILE* file_pointer)
{
    // write type

    char   typebytes[1] = {kTypeVector};
    size_t writeup      = fwrite(typebytes, sizeof(uint8_t), 1, file_pointer);

    for (int index = 0; index < vector->length; index++)
    {
	type_container_t* container = vector->data[index];

	if (container->type == kTypeString) mtfile_appendstringtofile(container->data, file_pointer);
	else if (container->type == kTypeVector) mtfile_appendvectortofile(container->data, file_pointer);
	else if (container->type == kTypeMap) mtfile_appendmaptofile(container->data, file_pointer);
    }

    // write closing 0

    char zerobytes[1] = {0};
    writeup           = fwrite(zerobytes, sizeof(uint8_t), 1, file_pointer);
}

/* appends map to opened file */

void mtfile_appendmaptofile(map_t* map, FILE* file_pointer)
{
    // write type

    char   typebytes[1] = {kTypeMap};
    size_t writeup      = fwrite(typebytes, sizeof(uint8_t), 1, file_pointer);

    vec_t* keys = VNEW();
    map_keys(map, keys);
    for (int index = 0; index < keys->length; index++)
    {
	type_container_t* container = MGET(map, keys->data[index]);

	if (container != NULL)
	{
	    str_t* keystring = str_frombytes(keys->data[index]);
	    mtfile_appendstringtofile(keystring, file_pointer);
	    REL(keystring);

	    if (container->type == kTypeString) mtfile_appendstringtofile(container->data, file_pointer);
	    else if (container->type == kTypeVector) mtfile_appendvectortofile(container->data, file_pointer);
	    else if (container->type == kTypeMap) mtfile_appendmaptofile(container->data, file_pointer);
	}
    }

    REL(keys);

    // write closing 0

    char zerobytes[1] = {0};
    writeup           = fwrite(zerobytes, sizeof(uint8_t), 1, file_pointer);
}

/* writes map to file recursively */

void mtfile_writetofile(map_t* map, char* path)
{
    remove(path);

    int fd = open(path, O_WRONLY | O_CREAT, 0666);

    if (fd > -1)
    {

	FILE* file_pointer = fdopen(fd, "a");

	if (file_pointer != NULL)
	{
	    mtfile_appendmaptofile(map, file_pointer);
	    fclose(file_pointer);
	}
    }
    else printf("cannot open path %s error : %s\n", path, strerror(errno));
}

/* reads string from file */

str_t* mtfile_readstring(FILE* file_pointer)
{
    char* bytes = CAL(sizeof(char) * 50, NULL, NULL);

    int index    = 0;
    int length   = 50;
    int nextbyte = fgetc(file_pointer);

    while (nextbyte != EOF && nextbyte != 0)
    {
	bytes[index++] = nextbyte;
	if (index == length)
	{
	    length += 50;
	    bytes = mem_realloc(bytes, sizeof(char) * length);
	}
	nextbyte = fgetc(file_pointer);
    }
    bytes[index++] = 0;

    str_t* result = str_frombytes(bytes);
    REL(bytes);
    return result;
}

/* reads vector from file */

vec_t* mtfile_readvector(FILE* file_pointer)
{
    vec_t* result   = VNEW();
    int    nextbyte = fgetc(file_pointer);

    while (nextbyte != EOF && nextbyte != 0)
    {
	if (nextbyte == kTypeMap)
	{
	    map_t*            map       = mtfile_readmap(file_pointer);
	    type_container_t* container = CAL(sizeof(type_container_t), NULL, NULL);
	    container->type             = kTypeMap;
	    container->data             = map;
	    VADD(result, container);
	    REL(container);
	}
	else if (nextbyte == kTypeVector)
	{
	    vec_t*            vector    = mtfile_readvector(file_pointer);
	    type_container_t* container = CAL(sizeof(type_container_t), NULL, NULL);
	    container->type             = kTypeVector;
	    container->data             = vector;
	    VADD(result, container);
	    REL(container);
	}
	else if (nextbyte == kTypeString)
	{
	    str_t*            string    = mtfile_readstring(file_pointer);
	    type_container_t* container = CAL(sizeof(type_container_t), NULL, NULL);
	    container->type             = kTypeString;
	    container->data             = string;
	    VADD(result, container);
	    REL(container);
	}

	// read next value's type

	nextbyte = fgetc(file_pointer);
    }
    return result;
}

/* reads map from file */

map_t* mtfile_readmap(FILE* file_pointer)
{
    map_t* result   = MNEW();
    int    nextbyte = fgetc(file_pointer);
    while (nextbyte != EOF && nextbyte != 0)
    {
	// read key

	str_t* key        = mtfile_readstring(file_pointer);
	char*  keycstring = str_new_cstring(key);

	// read value

	nextbyte = fgetc(file_pointer);
	if (nextbyte == kTypeMap)
	{
	    map_t*            map       = mtfile_readmap(file_pointer);
	    type_container_t* container = mtfile_defaultcontainer(kTypeMap, map);
	    MPUT(result, keycstring, container);
	    REL(container);
	    REL(map);
	}
	else if (nextbyte == kTypeVector)
	{
	    vec_t*            vector    = mtfile_readvector(file_pointer);
	    type_container_t* container = mtfile_defaultcontainer(kTypeVector, vector);
	    MPUT(result, keycstring, container);
	    REL(container);
	    REL(vector);
	}
	else if (nextbyte == kTypeString)
	{
	    str_t*            string    = mtfile_readstring(file_pointer);
	    type_container_t* container = mtfile_defaultcontainer(kTypeString, string);
	    MPUT(result, keycstring, container);
	    REL(container);
	    REL(string);
	}

	REL(keycstring);
	REL(key);

	// read key type

	nextbyte = fgetc(file_pointer);
    }
    return result;
}

// reads up file, returns map

map_t* mtfile_readfile(char* path)
{
    map_t* result = NULL;

    int fd = open(path, O_RDONLY);

    if (fd > -1)
    {

	FILE* file_pointer = fdopen(fd, "r");
	if (file_pointer != NULL)
	{
	    int nextbyte = fgetc(file_pointer);
	    if (nextbyte == kTypeMap)
	    {
		result = mtfile_readmap(file_pointer);
	    }
	    fclose(file_pointer);
	}
    }
    else printf("cannot open path %s error : %s\n", path, strerror(errno));

    return result;
}

/* reads up file, returns map */

vec_t* mtfile_readlines(char* path)
{
    vec_t* result      = VNEW();
    char   string[100] = {0};
    int    index       = 0;

    int fd = open(path, O_RDONLY);

    if (fd > -1)
    {

	FILE* file_pointer = fdopen(fd, "r");

	if (file_pointer != NULL)
	{
	    int nextbyte = fgetc(file_pointer);
	    while (nextbyte != EOF)
	    {
		if (nextbyte != '\n') string[index++] = (char) nextbyte;
		else
		{
		    char* copy = cstr_new_cstring(string);
		    VADD(result, copy);
		    REL(copy);
		    memset(string, 0, 100);
		    index = 0;
		}
		nextbyte = fgetc(file_pointer);
	    }
	    fclose(file_pointer);
	}
    }
    else printf("cannot open path %s error : %s\n", path, strerror(errno));

    return result;
}

/* destructs container */

void mtfile_defaultcontainer_destruct(void* pointer)
{
    type_container_t* container = pointer;
    REL(container->data);
}

/* creates container */

type_container_t* mtfile_defaultcontainer(char type, void* data)
{
    type_container_t* result = CAL(sizeof(type_container_t), mtfile_defaultcontainer_destruct, NULL);
    result->type             = type;
    result->data             = RET(data);
    return result;
}

#endif
