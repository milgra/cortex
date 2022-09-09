#ifndef mtfile_h
#define mtfile_h

#include "mtmap.c"
#include "mtstr.c"
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

void              mtfile_writetofile(mtmap_t* map, char* path);
mtmap_t*          mtfile_readfile(char* path);
void              mtfile_appendstringtofile(mtstr_t* string, FILE* file_pointer);
void              mtfile_appendvectortofile(mtvec_t* vector, FILE* file_pointer);
void              mtfile_appendmaptofile(mtmap_t* map, FILE* file_pointer);
mtmap_t*          mtfile_readmap(FILE* file_pointer);
mtstr_t*          mtfile_readstring(FILE* file_pointer);
mtvec_t*          mtfile_readvector(FILE* file_pointer);
mtvec_t*          mtfile_readlines(char* path);
type_container_t* mtfile_defaultcontainer(char type, void* data);

#endif

#if __INCLUDE_LEVEL__ == 0

#include "mtcstr.c"
#include "mtmem.c"
#include <string.h>

/* appends string to opened file */

void mtfile_appendstringtofile(mtstr_t* string, FILE* file_pointer)
{
    // write type

    size_t writeup;
    char   typebytes[1] = {kTypeString};
    writeup             = fwrite(typebytes, sizeof(uint8_t), 1, file_pointer);

    // write bytes

    char* bytes = mtstr_bytes(string);
    writeup     = fwrite(bytes, sizeof(uint8_t), string->length_bytes, file_pointer);

    // write closing 0

    char zerobytes[1] = {0};
    writeup           = fwrite(zerobytes, sizeof(uint8_t), 1, file_pointer);

    // cleanup

    mtmem_release(bytes);
}

/* appends vector to opened file */

void mtfile_appendvectortofile(mtvec_t* vector, FILE* file_pointer)
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

void mtfile_appendmaptofile(mtmap_t* map, FILE* file_pointer)
{
    // write type

    char   typebytes[1] = {kTypeMap};
    size_t writeup      = fwrite(typebytes, sizeof(uint8_t), 1, file_pointer);

    mtvec_t* keys = mtmap_keys(map);
    for (int index = 0; index < keys->length; index++)
    {
	type_container_t* container = mtmap_get(map, keys->data[index]);

	if (container != NULL)
	{
	    mtstr_t* keystring = mtstr_frombytes(keys->data[index]);
	    mtfile_appendstringtofile(keystring, file_pointer);
	    mtmem_release(keystring);

	    if (container->type == kTypeString) mtfile_appendstringtofile(container->data, file_pointer);
	    else if (container->type == kTypeVector) mtfile_appendvectortofile(container->data, file_pointer);
	    else if (container->type == kTypeMap) mtfile_appendmaptofile(container->data, file_pointer);
	}
    }

    mtmem_release(keys);

    // write closing 0

    char zerobytes[1] = {0};
    writeup           = fwrite(zerobytes, sizeof(uint8_t), 1, file_pointer);
}

/* writes map to file recursively */

void mtfile_writetofile(mtmap_t* map, char* path)
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

mtstr_t* mtfile_readstring(FILE* file_pointer)
{
    char* bytes = mtmem_calloc(sizeof(char) * 50, NULL);

    int index    = 0;
    int length   = 50;
    int nextbyte = fgetc(file_pointer);

    while (nextbyte != EOF && nextbyte != 0)
    {
	bytes[index++] = nextbyte;
	if (index == length)
	{
	    length += 50;
	    bytes = mtmem_realloc(bytes, sizeof(char) * length);
	}
	nextbyte = fgetc(file_pointer);
    }
    bytes[index++] = 0;

    mtstr_t* result = mtstr_frombytes(bytes);
    mtmem_release(bytes);
    return result;
}

/* reads vector from file */

mtvec_t* mtfile_readvector(FILE* file_pointer)
{
    mtvec_t* result   = mtvec_alloc();
    int      nextbyte = fgetc(file_pointer);

    while (nextbyte != EOF && nextbyte != 0)
    {
	if (nextbyte == kTypeMap)
	{
	    mtmap_t*          map       = mtfile_readmap(file_pointer);
	    type_container_t* container = mtmem_calloc(sizeof(type_container_t), NULL);
	    container->type             = kTypeMap;
	    container->data             = map;
	    mtvec_add(result, container);
	    mtmem_release(container);
	}
	else if (nextbyte == kTypeVector)
	{
	    mtvec_t*          vector    = mtfile_readvector(file_pointer);
	    type_container_t* container = mtmem_calloc(sizeof(type_container_t), NULL);
	    container->type             = kTypeVector;
	    container->data             = vector;
	    mtvec_add(result, container);
	    mtmem_release(container);
	}
	else if (nextbyte == kTypeString)
	{
	    mtstr_t*          string    = mtfile_readstring(file_pointer);
	    type_container_t* container = mtmem_calloc(sizeof(type_container_t), NULL);
	    container->type             = kTypeString;
	    container->data             = string;
	    mtvec_add(result, container);
	    mtmem_release(container);
	}

	// read next value's type

	nextbyte = fgetc(file_pointer);
    }
    return result;
}

/* reads map from file */

mtmap_t* mtfile_readmap(FILE* file_pointer)
{
    mtmap_t* result   = mtmap_alloc();
    int      nextbyte = fgetc(file_pointer);
    while (nextbyte != EOF && nextbyte != 0)
    {
	// read key

	mtstr_t* key        = mtfile_readstring(file_pointer);
	char*    keycstring = mtstr_bytes(key);

	// read value

	nextbyte = fgetc(file_pointer);
	if (nextbyte == kTypeMap)
	{
	    mtmap_t*          map       = mtfile_readmap(file_pointer);
	    type_container_t* container = mtfile_defaultcontainer(kTypeMap, map);
	    mtmap_put(result, keycstring, container);
	    mtmem_release(container);
	    mtmem_release(map);
	}
	else if (nextbyte == kTypeVector)
	{
	    mtvec_t*          vector    = mtfile_readvector(file_pointer);
	    type_container_t* container = mtfile_defaultcontainer(kTypeVector, vector);
	    mtmap_put(result, keycstring, container);
	    mtmem_release(container);
	    mtmem_release(vector);
	}
	else if (nextbyte == kTypeString)
	{
	    mtstr_t*          string    = mtfile_readstring(file_pointer);
	    type_container_t* container = mtfile_defaultcontainer(kTypeString, string);
	    mtmap_put(result, keycstring, container);
	    mtmem_release(container);
	    mtmem_release(string);
	}

	mtmem_release(keycstring);
	mtmem_release(key);

	// read key type

	nextbyte = fgetc(file_pointer);
    }
    return result;
}

// reads up file, returns map

mtmap_t* mtfile_readfile(char* path)
{
    mtmap_t* result = NULL;

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

mtvec_t* mtfile_readlines(char* path)
{
    mtvec_t* result      = mtvec_alloc();
    char     string[100] = {0};
    int      index       = 0;

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
		    char* copy = mtcstr_fromcstring(string);
		    mtvec_add(result, copy);
		    mtmem_release(copy);
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
    mtmem_release(container->data);
}

/* creates container */

type_container_t* mtfile_defaultcontainer(char type, void* data)
{
    type_container_t* result = mtmem_calloc(sizeof(type_container_t), mtfile_defaultcontainer_destruct);
    result->type             = type;
    result->data             = mtmem_retain(data);
    return result;
}

#endif
