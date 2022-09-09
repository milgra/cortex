#ifndef script_h
#define script_h

#include "generator.c"
#include "maincube.c"
#include "mtbus.c"
#include "mtstr.c"
#include "mtvec.c"

typedef struct _script_t script_t;
struct _script_t
{

    mtvec_t* list;
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

    script.list  = mtvec_alloc();
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

    mtvec_reset(script.list);

    script.index      = 0;
    script.frame_next = 0;

    REL(script.label);
    script.label = NULL;
}

void script_load_item(
    uint32_t index)
{

    if (script.list->length == 0) return;

    mtmap_t* item = script.list->data[index];

    mtstr_t* next = mtmap_get(item, "next");

    if (next != NULL)
    {

	mtbus_notify(
	    "SCN",
	    "NEXTSCENE",
	    NULL);
	return;
    }

    generator_setup(item);

    mtstr_t* label = mtmap_get(item, "label");

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
	script.label = mtstr_bytes(label);
    }
    else
    {
	REL(script.label);
	script.label = NULL;
    }

    // cube

    mtstr_t* cube = mtmap_get(item, "cube");

    if (cube != NULL)
    {
	if (cube->codepoints[0] == '0') maincube.visible = 0;
	else maincube.visible = 1;
    }

    // get next frame

    if (index + 1 < script.list->length)
    {
	mtmap_t* nextline   = script.list->data[index + 1];
	mtstr_t* frametoken = mtmap_get(nextline, "frame");
	if (frametoken != NULL) script.frame_next = mtstr_intvalue(frametoken);
    }
}

void script_load(
    char* descriptor)
{

    mtstr_t* string = mtstr_frombytes(descriptor);
    mtvec_t* lines  = mtstr_split(string, '\n');

    for (int lineindex = 0;
	 lineindex < lines->length;
	 lineindex++)
    {

	mtstr_t* line = lines->data[lineindex];
	mtmap_t* map  = mtstr_tokenize(line);
	mtvec_add(script.list, map);
	mtmem_release(map);
    }

    mtmem_release(string);
    mtmem_release(lines);

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
