#ifndef cmd_h
#define cmd_h

#include "zc_vector.c"
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
    vec_t* commands;
    vec_t* delayed;
    char   is_iterating;
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
#include "zc_memory.c"
#include <string.h>

/* alloc command */

cmd_t* cmd_alloc(char* name, void* data, void* args)
{
    assert(name != NULL);
    cmd_t* cmd = CAL(sizeof(cmd_t), cmd_dealloc, NULL);

    cmd->name = mtcstr_fromcstring(name);
    cmd->data = RET(data);
    cmd->args = RET(args);

    return cmd;
}

/* dealloc command */

void cmd_dealloc(void* pointer)
{
    cmd_t* cmd = pointer;

    if (cmd->name != NULL) REL(cmd->name);
    if (cmd->data != NULL) REL(cmd->data);
    if (cmd->args != NULL) REL(cmd->args);
}

/* alloc command queue */

cmdqueue_t* cmdqueue_alloc()
{
    cmdqueue_t* queue = CAL(sizeof(cmdqueue_t), cmdqueue_dealloc, NULL);

    queue->commands = VNEW();
    queue->delayed  = VNEW();

    return queue;
}

/* dealloc command queue */

void cmdqueue_dealloc(void* pointer)
{
    cmdqueue_t* queue = pointer;

    REL(queue->commands);
    REL(queue->delayed);
}

/* sets iteration */

void cmdqueue_setiteration(cmdqueue_t* queue)
{
    queue->is_iterating = 1;
}

/* reset command queue */

void cmdqueue_reset(cmdqueue_t* queue)
{
    vec_reset(queue->commands);
    queue->is_iterating = 0;
}

/* add command to queue */

void cmdqueue_add(cmdqueue_t* queue, char* name, void* data, void* args)
{
    cmd_t* cmd = cmd_alloc(name, data, args);
    VADD(queue->commands, cmd);
    REL(cmd);
}

/* add command to queue */

void cmdqueue_addcmd(cmdqueue_t* queue, cmd_t* cmd)
{
    VADD(queue->commands, cmd);
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
    VADD(queue->delayed, cmd);
    REL(cmd);
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
		VADD(queue->commands, cmd);
		VREM(queue->delayed, cmd);
		return;
	    }
	}
    }
}

#endif
