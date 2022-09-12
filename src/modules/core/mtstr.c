#ifndef mtstr_h
#define mtstr_h

#include "zc_map.c"
#include "zc_vector.c"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _mtstr_t mtstr_t;
struct _mtstr_t
{
    uint32_t  length;
    uint32_t  length_real;
    uint32_t  length_bytes;
    uint32_t* codepoints;
};

mtstr_t* mtstr_alloc(void);
void     mtstr_dealloc(void* string);
void     mtstr_reset(mtstr_t* string);
mtstr_t* mtstr_fromformat(char* format, ...);
mtstr_t* mtstr_frombytes(char* bytes);
mtstr_t* mtstr_fromstring(mtstr_t* string);
mtstr_t* mtstr_substring(mtstr_t* string, int start, int end);
mtstr_t* mtstr_replace(mtstr_t* string, mtstr_t* newstring, int start, int end);
mtstr_t* mtstr_compactemojis(mtstr_t* string);
void     mtstr_addstring(mtstr_t* mtstra, mtstr_t* mtstrb);
void     mtstr_addbytearray(mtstr_t* string, char* bytearray);
void     mtstr_addcodepoint(mtstr_t* string, uint32_t codepoint);
void     mtstr_removecodepointatindex(mtstr_t* string, uint32_t index);
void     mtstr_removecodepointsinrange(mtstr_t* string, uint32_t start, uint32_t end);
void     mtstr_replacecodepoints(mtstr_t* string, uint32_t oldcp, uint32_t newcp);
int8_t   mtstr_compare(mtstr_t* mtstra, mtstr_t* mtstrb);
int      mtstr_intvalue(mtstr_t* string);
float    mtstr_floatvalue(mtstr_t* string);
uint32_t mtstr_unsignedvalue(mtstr_t* string);
vec_t*   mtstr_split(mtstr_t* string, char character);
map_t*   mtstr_tokenize(mtstr_t* descriptor);
uint32_t mtstr_find(mtstr_t* string, mtstr_t* substring, uint32_t from);
char*    mtstr_bytes(mtstr_t* string);

#endif
#if __INCLUDE_LEVEL__ == 0

#include "zc_memory.c"
#include <string.h>

#define UTF8_BOM "\xEF\xBB\xBF"

/* creates string */

mtstr_t* mtstr_alloc()
{
    mtstr_t* string = CAL(sizeof(mtstr_t), mtstr_dealloc, NULL);

    string->length       = 0;  // current length of codepoint array
    string->length_real  = 10; // backing length of codepoint array
    string->length_bytes = 0;  // needed length of byte array for all codepoints
    string->codepoints   = CAL(string->length_real * sizeof(uint32_t), NULL, NULL);

    return string;
}

/* deletes string */

void mtstr_dealloc(void* pointer)
{
    mtstr_t* string = pointer;
    REL(string->codepoints);
}

/* resets string */

void mtstr_reset(mtstr_t* string)
{
    string->length       = 0;
    string->length_real  = 10;
    string->length_bytes = 0;
    string->codepoints   = mem_realloc(string->codepoints, string->length_real * sizeof(uint32_t));
    memset(string->codepoints, 0, string->length_real * sizeof(uint32_t));
}

/* creates string from utf8 bytearray */

mtstr_t* mtstr_fromformat(char* format, ...)
{
    va_list ap;
    char*   text;
    size_t  length = strlen(format);

    va_start(ap, format);
    for (text = format; text != NULL; text = va_arg(ap, char*)) length += strlen(text);
    length += 1;
    va_end(ap);

    char* result = CAL(sizeof(char) * length, NULL, NULL);
    va_start(ap, format);
    vsnprintf(result, length, format, ap);
    va_end(ap);

    mtstr_t* resstring = mtstr_frombytes(result);
    REL(result);

    return resstring;
}

/* creates string from utf8 bytearray */

mtstr_t* mtstr_frombytes(char* bytes)
{
    if (bytes != NULL)
    {
	mtstr_t* string = mtstr_alloc();
	mtstr_addbytearray(string, bytes);
	return string;
    }
    else return NULL;
}

/* creates string from string */

mtstr_t* mtstr_fromstring(mtstr_t* string)
{
    mtstr_t* result = mtstr_alloc();
    mtstr_addstring(result, string);
    return result;
}

/* returns substring of string */

mtstr_t* mtstr_substring(mtstr_t* string, int start, int end)
{
    mtstr_t* result = mtstr_alloc();

    for (int index = start; index < end; index++)
    {
	mtstr_addcodepoint(result, string->codepoints[index]);
    }

    return result;
}

/* replaces substring in string */

mtstr_t* mtstr_replace(mtstr_t* string, mtstr_t* newstring, int start, int end)
{
    mtstr_t* part1 = mtstr_substring(string, 0, start);
    mtstr_t* part2 = mtstr_substring(string, end, string->length);

    mtstr_addstring(part1, newstring);
    mtstr_addstring(part1, part2);

    return part1;
}

/* compact :) and :( to their unicode codepoints */

mtstr_t* mtstr_compactemojis(mtstr_t* string)
{
    mtstr_t* result = mtstr_alloc();

    for (int index = 0; index < string->length; index++)
    {
	if (index < string->length - 1)
	{
	    if (string->codepoints[index] == ':' && string->codepoints[index + 1] == ')')
	    {
		mtstr_addcodepoint(result, 0x1F601);
		index++;
	    }
	    else if (string->codepoints[index] == ':' && string->codepoints[index + 1] == '(')
	    {
		mtstr_addcodepoint(result, 0x1F61E);
		index++;
	    }
	    else mtstr_addcodepoint(result, string->codepoints[index]);
	}
	else mtstr_addcodepoint(result, string->codepoints[index]);
    }
    return result;
}

/* adds string to string */

void mtstr_addstring(mtstr_t* mtstra, mtstr_t* mtstrb)
{
    if (mtstrb != NULL)
    {
	uint32_t newlength       = mtstra->length + mtstrb->length;
	uint32_t newlength_real  = mtstra->length_real + mtstrb->length_real;
	uint32_t newlength_bytes = mtstra->length_bytes + mtstrb->length_bytes;

	mtstra->codepoints = mem_realloc(mtstra->codepoints, sizeof(uint32_t) * newlength_real);
	memcpy((void*) (mtstra->codepoints + mtstra->length), (void*) mtstrb->codepoints, mtstrb->length * sizeof(uint32_t));

	mtstra->length       = newlength;
	mtstra->length_real  = newlength_real;
	mtstra->length_bytes = newlength_bytes;
    }
}

/* returns byte count for codepoint */

uint8_t mtstrgetcodebytelength(uint32_t codepoint)
{
    uint8_t codelength = 4;
    if (codepoint < 0x80) codelength = 1;
    else if (codepoint < 0x800) codelength = 2;
    else if (codepoint < 0x1000) codelength = 3;
    return codelength;
}

/* adds buffer containing bytes for one codepoint */

void mtstr_addbuffer(mtstr_t* string, char* buffer, char length)
{
    // filter byte order mark
    if (strcmp(buffer, UTF8_BOM) != 0)
    {
	uint32_t codepoint = 0;
	// extract codepoint
	if (length == 1) codepoint = buffer[0] & 0x7F;
	else if (length == 2) codepoint = (buffer[0] & 0x1F) << 6 | (buffer[1] & 0x3F);
	else if (length == 3) codepoint = (buffer[0] & 0xF) << 12 | (buffer[1] & 0x3F) << 6 | (buffer[2] & 0x3F);
	else if (length == 4) codepoint = (buffer[0] & 0x7) << 18 | (buffer[1] & 0x3F) << 12 | (buffer[2] & 0x3F) << 6 | (buffer[3] & 0x3F);
	// add codepoint
	mtstr_addcodepoint(string, codepoint);
    }
}

/* adds utf8 encoded byte array */

void mtstr_addbytearray(mtstr_t* string, char* bytearray)
{
    char buffer[4]       = {0};
    char buffer_position = 0;
    while (*bytearray != 0)
    {
	// checking unicode closing characters or last character
	if ((*bytearray & 0xFF) >> 7 == 0 || (*bytearray & 0xFF) >> 6 == 3)
	{
	    if (buffer_position > 0)
	    {
		mtstr_addbuffer(string, buffer, buffer_position);
		// reset unicode buffer
		memset(&buffer, 0, 4);
		buffer_position = 0;
	    }
	}
	// storing actual byte in unicode codepoint buffer
	buffer[buffer_position++] = *bytearray;
	// step in byte array
	bytearray += 1;
	// invalid utf sequence, aborting
	if (buffer_position == 5) return;
    }
    // add remaining buffer content
    if (buffer_position > 0) mtstr_addbuffer(string, buffer, buffer_position);
}

/* adds code point */

void mtstr_addcodepoint(mtstr_t* string, uint32_t codepoint)
{
    uint8_t codelength = mtstrgetcodebytelength(codepoint);

    // expand
    if (string->length_real == string->length)
    {
	string->codepoints = mem_realloc(string->codepoints, sizeof(uint32_t) * (string->length_real + 10));
	string->length_real += 10;
    }

    string->codepoints[string->length] = codepoint;
    string->length += 1;
    string->length_bytes += codelength;
}

/* removes codepoint */

void mtstr_removecodepointatindex(mtstr_t* string, uint32_t index)
{
    uint32_t codepoint  = string->codepoints[index];
    uint8_t  codelength = mtstrgetcodebytelength(codepoint);

    string->length_bytes -= codelength;
    memmove(string->codepoints + index, string->codepoints + index + 1, (string->length - index - 1) * sizeof(uint32_t));
    string->length -= 1;
}

/* removes codepoints in range */

void mtstr_removecodepointsinrange(mtstr_t* string, uint32_t start, uint32_t end)
{
    if (end > string->length) end = string->length;

    for (int index = start; index < end; index++)
    {
	uint32_t codepoint  = string->codepoints[index];
	uint8_t  codelength = mtstrgetcodebytelength(codepoint);
	string->length_bytes -= codelength;
    }

    if (end < string->length)
    {
	memmove(
	    string->codepoints + start,
	    string->codepoints + end + 1,
	    (string->length - end - 1) * sizeof(uint32_t));
    }

    string->length -= end - start + 1;
}

/* replaces codepoints */

void mtstr_replacecodepoints(mtstr_t* string, uint32_t oldcp, uint32_t newcp)
{
    for (int index = 0; index < string->length; index++)
    {
	if (string->codepoints[index] == oldcp) string->codepoints[index] = newcp;
    }
}

/* compares two string */

int8_t mtstr_compare(mtstr_t* mtstra, mtstr_t* mtstrb)
{
    char* bytes_a = mtstr_bytes(mtstra);
    char* bytes_b = mtstr_bytes(mtstrb);

    int8_t result = strcmp(bytes_a, bytes_b);

    REL(bytes_a);
    REL(bytes_b);

    return result;
}

/* returns intvalue */

int mtstr_intvalue(mtstr_t* string)
{
    char* viewindexc = mtstr_bytes(string);
    int   viewindex  = atoi(viewindexc);
    REL(viewindexc);
    return viewindex;
}

/* returns floatvalue */

float mtstr_floatvalue(mtstr_t* string)
{
    char* viewindexc = mtstr_bytes(string);
    float viewindex  = atof(viewindexc);
    REL(viewindexc);
    return viewindex;
}

/* returns unsigned value */

uint32_t mtstr_unsignedvalue(mtstr_t* string)
{
    char*         valuec = mtstr_bytes(string);
    unsigned long value  = strtoul(valuec, NULL, 0);
    REL(valuec);
    return (uint32_t) value;
}

/* splits string at codepoint to a vector */

vec_t* mtstr_split(mtstr_t* string, char codepoint)
{
    vec_t*   vector  = VNEW();
    mtstr_t* segment = mtstr_alloc();
    for (int index = 0; index < string->length; index++)
    {
	if (string->codepoints[index] == codepoint)
	{
	    // add word to result, create new word
	    if (segment->length > 0)
	    {
		VADD(vector, segment);
		REL(segment);
		segment = mtstr_alloc();
	    }
	}
	else mtstr_addcodepoint(segment, string->codepoints[index]);
    }
    // add word to result
    if (segment->length > 0) VADD(vector, segment);
    REL(segment);
    return vector;
}

/* splits string at spaces and creates key-value pairs */

map_t* mtstr_tokenize(mtstr_t* descriptor)
{
    map_t* map    = MNEW();
    vec_t* tokens = mtstr_split(descriptor, ' ');
    for (int index = 0; index < tokens->length; index += 2)
    {
	char* key = mtstr_bytes(tokens->data[index]);
	MPUT(map, key, tokens->data[index + 1]);
	REL(key);
    }
    REL(tokens);
    return map;
}

/* finds substring in string from given index */

uint32_t mtstr_find(mtstr_t* string, mtstr_t* substring, uint32_t from)
{
    if (string == NULL) return UINT32_MAX;
    if (substring == NULL) return UINT32_MAX;
    if (string->length < substring->length) return UINT32_MAX;

    for (uint32_t index = from; index < string->length - substring->length + 1; index++)
    {
	if (string->codepoints[index] == substring->codepoints[0])
	{
	    uint32_t count;
	    for (count = 1; count < substring->length; count++)
	    {
		if (string->codepoints[index + count] != substring->codepoints[count]) break;
	    }
	    if (count == substring->length) return index;
	}
    }
    return UINT32_MAX;
}

/* returns backing bytearray */

char* mtstr_bytes(mtstr_t* string)
{
    if (string == NULL) return NULL;
    char*    bytes    = CAL((string->length_bytes + 1) * sizeof(char), NULL, NULL);
    uint32_t position = 0;
    for (int index = 0; index < string->length; index++)
    {
	uint32_t codepoint = string->codepoints[index];
	if (codepoint < 0x80)
	{
	    bytes[position++] = codepoint;
	}
	else if (codepoint < 0x800)
	{
	    bytes[position++] = (codepoint >> 6) | 0xC0;
	    bytes[position++] = (codepoint & 0x3F) | 0x80;
	}
	else if (codepoint < 0x1000)
	{
	    bytes[position++] = (codepoint >> 12) | 0xE0;
	    bytes[position++] = ((codepoint >> 6) & 0x3F) | 0x80;
	    bytes[position++] = (codepoint & 0x3F) | 0x80;
	}
	else
	{
	    bytes[position++] = (codepoint >> 18) | 0xF0;
	    bytes[position++] = ((codepoint >> 12) & 0x3F) | 0x80;
	    bytes[position++] = ((codepoint >> 6) & 0x3F) | 0x80;
	    bytes[position++] = (codepoint & 0x3F) | 0x80;
	}
    }
    return bytes;
}

#endif
