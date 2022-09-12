#ifndef script_h
#define script_h

#include "generator.c"
#include "maincube.c"
#include "mtbus.c"
#include "str_util.c"
#include "zc_string.c"
#include "zc_vector.c"

typedef struct _script_t script_t;
struct _script_t
{

    vec_t*   list;
    uint32_t index;
    uint32_t frame_next;

    char* label;
};

void script_init(
    void);

void script_free(
    void);

void script_reset(
    void);

void script_load(
    char* descriptor);

void script_update(
    uint32_t frame);

extern script_t script;

#endif

#if __INCLUDE_LEVEL__ == 0

script_t script;

void script_init(
    void)
{

    script.list  = VNEW();
    script.label = NULL;
}

void script_free(
    void)
{

    REL(script.list);
}

void script_reset(
    void)
{

    vec_reset(script.list);

    script.index      = 0;
    script.frame_next = 0;

    REL(script.label);
    script.label = NULL;
}

void script_load_item(
    uint32_t index)
{

    if (script.list->length == 0) return;

    map_t* item = script.list->data[index];

    str_t* next = MGET(item, "next");

    if (next != NULL)
    {

	mtbus_notify(
	    "SCN",
	    "NEXTSCENE",
	    NULL);
	return;
    }

    generator_setup(item);

    str_t* label = MGET(item, "label");

    if (label != NULL)
    {

	for (int index = 0;
	     index < label->length;
	     index++)
	{

	    if (label->codepoints[index] == '_')
	    {

		label->codepoints[index] = ' ';
	    }
	}

	REL(script.label);
	script.label = str_new_cstring(label);
    }
    else
    {
	REL(script.label);
	script.label = NULL;
    }

    // cube

    str_t* cube = MGET(item, "cube");

    if (cube != NULL)
    {
	if (cube->codepoints[0] == '0') maincube.visible = 0;
	else maincube.visible = 1;
    }

    // get next frame

    if (index + 1 < script.list->length)
    {
	map_t* nextline   = script.list->data[index + 1];
	str_t* frametoken = MGET(nextline, "frame");
	if (frametoken != NULL) script.frame_next = str_intvalue(frametoken);
    }
}

void script_load(
    char* descriptor)
{

    str_t* string = str_frombytes(descriptor);
    vec_t* lines  = str_split(string, '\n');

    for (int lineindex = 0;
	 lineindex < lines->length;
	 lineindex++)
    {

	str_t* line = lines->data[lineindex];
	map_t* map  = str_tokenize(line);
	vec_add(script.list, map);
	REL(map);
    }

    REL(string);
    REL(lines);

    script_load_item(0);
}

void script_update(
    uint32_t frame)
{

    if (frame == script.frame_next)
    {

	script_load_item(++script.index);
    }
}

#endif
