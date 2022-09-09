#ifndef cmd_h
#define cmd_h

#include "mtvec.c"
#include <stdio.h>

typedef struct _cmd_t cmd_t;
struct _cmd_t
{
    char*    name;
    void*    data;
    void*    args;
    uint32_t time;
};

cmd_t* cmd_alloc(char* name, void* view, void* arguments);
void   cmd_dealloc(void* cmd);

typedef struct _cmdqueue_t cmdqueue_t;
struct _cmdqueue_t
{
    mtvec_t* commands;
    mtvec_t* delayed;
    char     is_iterating;
};

cmdqueue_t* cmdqueue_alloc(void);
void        cmdqueue_dealloc(void* pointer);
void        cmdqueue_setiteration(cmdqueue_t* queue);
void        cmdqueue_add(cmdqueue_t* queue, char* name, void* data, void* args);
void        cmdqueue_addcmd(cmdqueue_t* queue, cmd_t* cmd);
void        cmdqueue_addonce(cmdqueue_t* queue, char* name, void* data, void* args);
void        cmdqueue_delay(cmdqueue_t* queue, char* name, void* data, void* args, uint32_t time);
void        cmdqueue_timer(cmdqueue_t* timer, uint32_t time);
void        cmdqueue_reset(cmdqueue_t* queue);

#endif

#if __INCLUDE_LEVEL__ == 0

#include "../core/mtcstr.c"
#include "../core/mtmem.c"
#include "../core/mtstr.c"
#include <string.h>

/* alloc command */

cmd_t* cmd_alloc(char* name, void* data, void* args)
{
    assert(name != NULL);
    cmd_t* cmd = mtmem_calloc(sizeof(cmd_t), cmd_dealloc);

    cmd->name = mtcstr_fromcstring(name);
    cmd->data = mtmem_retain(data);
    cmd->args = mtmem_retain(args);

    return cmd;
}

/* dealloc command */

void cmd_dealloc(void* pointer)
{
    cmd_t* cmd = pointer;

    if (cmd->name != NULL) mtmem_release(cmd->name);
    if (cmd->data != NULL) mtmem_release(cmd->data);
    if (cmd->args != NULL) mtmem_release(cmd->args);
}

/* alloc command queue */

cmdqueue_t* cmdqueue_alloc()
{
    cmdqueue_t* queue = mtmem_calloc(sizeof(cmdqueue_t), cmdqueue_dealloc);

    queue->commands = mtvec_alloc();
    queue->delayed  = mtvec_alloc();

    return queue;
}

/* dealloc command queue */

void cmdqueue_dealloc(void* pointer)
{
    cmdqueue_t* queue = pointer;

    mtmem_release(queue->commands);
    mtmem_release(queue->delayed);
}

/* sets iteration */

void cmdqueue_setiteration(cmdqueue_t* queue)
{
    queue->is_iterating = 1;
}

/* reset command queue */

void cmdqueue_reset(cmdqueue_t* queue)
{
    mtvec_reset(queue->commands);
    queue->is_iterating = 0;
}

/* add command to queue */

void cmdqueue_add(cmdqueue_t* queue, char* name, void* data, void* args)
{
    cmd_t* cmd = cmd_alloc(name, data, args);
    mtvec_add(queue->commands, cmd);
    mtmem_release(cmd);
}

/* add command to queue */

void cmdqueue_addcmd(cmdqueue_t* queue, cmd_t* cmd)
{
    mtvec_add(queue->commands, cmd);
}

/* add command to queue if not exists in queue */

void cmdqueue_addonce(cmdqueue_t* queue, char* name, void* data, void* args)
{
    for (int index = 0; index < queue->commands->length; index++)
    {
	cmd_t* command = queue->commands->data[index];
	if (strcmp(command->name, name) == 0 && data == command->data) return;
    }

    cmdqueue_add(queue, name, data, args);
}

/* delay command with given time */

void cmdqueue_delay(cmdqueue_t* queue, char* name, void* data, void* args, uint32_t time)
{
    cmd_t* cmd = cmd_alloc(name, data, args);
    cmd->time  = time;
    mtvec_add(queue->delayed, cmd);
    mtmem_release(cmd);
}

/* command queue iteration */

void cmdqueue_timer(cmdqueue_t* queue, uint32_t time)
{
    if (queue->delayed->length > 0)
    {
	for (int index = 0; index < queue->delayed->length; index++)
	{
	    cmd_t* cmd = queue->delayed->data[index];

	    if (cmd->time == time)
	    {
		mtvec_add(queue->commands, cmd);
		mtvec_remove(queue->delayed, cmd);
		return;
	    }
	}
    }
}

#endif
