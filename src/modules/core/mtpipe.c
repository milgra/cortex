#ifndef mtpipe_h
#define mtpipe_h

#include "zc_memory.c"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>

typedef struct mtpipe_t mtpipe_t;
struct mtpipe_t
{
    char*  flags;
    void** boxes;

    uint32_t size;
    uint32_t read_index;
    uint32_t write_index;
};

mtpipe_t* mtpipe_alloc(uint32_t size);
void      mtpipe_dealloc(void* pointer);
char      mtpipe_send(mtpipe_t* boxes, void* data);
void*     mtpipe_recv(mtpipe_t* boxes);
void      mtpipe_test(void);

#endif

#if __INCLUDE_LEVEL__ == 0

/* create pipe */

mtpipe_t* mtpipe_alloc(uint32_t size)
{
    mtpipe_t* boxes = CAL(sizeof(mtpipe_t), mtpipe_dealloc, NULL);

    boxes->flags       = CAL(sizeof(char) * size, NULL, NULL);
    boxes->boxes       = CAL(sizeof(void*) * size, NULL, NULL);
    boxes->size        = size;
    boxes->read_index  = 0;
    boxes->write_index = 0;

    return boxes;
}

/* destructor */

void mtpipe_dealloc(void* pointer)
{
    assert(pointer != NULL);

    mtpipe_t* boxes = pointer;

    REL(boxes->flags);
    REL(boxes->boxes);
}

/* send data to other thread */

char mtpipe_send(mtpipe_t* boxes, void* data)
{
    assert(boxes != NULL);
    assert(data != NULL);

    // wait for the box to get empty

    if (boxes->flags[boxes->write_index] == 0)
    {
	boxes->boxes[boxes->write_index] = data;

	// set flag, it doesn't have to be atomic, only the last bit counts

	boxes->flags[boxes->write_index] = 1;

	// increment write index, doesn't have to be atomic, this thread uses it only

	boxes->write_index += 1;
	if (boxes->write_index == boxes->size) boxes->write_index = 0;

	return 1;
    }

    return 0;
}

/* receive data from other thread */

void* mtpipe_recv(mtpipe_t* boxes)
{
    assert(boxes != NULL);

    if (boxes->flags[boxes->read_index] == 1)
    {
	void* result = boxes->boxes[boxes->read_index];

	// empty box

	boxes->boxes[boxes->read_index] = NULL;

	// set flag, it doesn't have to be atomic, only the last bit counts

	boxes->flags[boxes->read_index] = 0;

	// increment read index, it doesn't have to be atomic, this thread

	boxes->read_index += 1;
	if (boxes->read_index == boxes->size) boxes->read_index = 0;

	return result;
    }

    return NULL;
}

//
//  TEST
//

#define kBoxesTestThreads 10

void send_test(mtpipe_t* boxes)
{
    uint32_t counter = 0;
    while (1)
    {
	uint32_t* number = CAL(sizeof(uint32_t), NULL, NULL);
	*number          = counter;
	char success     = mtpipe_send(boxes, number);
	if (success == 0) REL(number);
	else counter += 1;
	if (counter == UINT32_MAX - 1) counter = 0;
	//            struct timespec time;
	//            time.tv_sec = 0;
	//            time.tv_nsec = rand() % 100000;
	//            nanosleep(&time , (struct timespec *)NULL);
    }
}

void recv_test(mtpipe_t* boxes)
{
    uint32_t last = 0;
    while (1)
    {
	uint32_t* number = mtpipe_recv(boxes);
	if (number != NULL)
	{
	    if (*number != last) printf("index error!!!");
	    REL(number);
	    last += 1;
	    if (last == UINT32_MAX - 1) last = 0;
	    if (last % 100000 == 0) printf("%x OK %u %u", (uint32_t) boxes, last, UINT32_MAX);
	    //                struct timespec time;
	    //                time.tv_sec = 0;
	    //                time.tv_nsec = rand() % 100000;
	    //                nanosleep(&time , (struct timespec *)NULL);
	}
    }
}

mtpipe_t** testarray;

void mtpipe_test()
{
    testarray = CAL(sizeof(mtpipe_t) * kBoxesTestThreads, NULL, NULL);

    for (int index = 0; index < kBoxesTestThreads; index++)
    {
	testarray[index] = mtpipe_alloc(100);
	pthread_t thread;
	pthread_create(&thread, NULL, (void*) send_test, testarray[index]);
	pthread_create(&thread, NULL, (void*) recv_test, testarray[index]);
    }
}

#endif
