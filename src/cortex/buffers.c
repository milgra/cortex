/* Vertex buffers for different layers of the scene */

#ifndef buffers_h
#define buffers_h

#include "floatbuffer.c"
#include "renderer.c"

typedef struct _buffers_t buffers_t;
struct _buffers_t
{

    floatbuffer_t* buffera;
    floatbuffer_t* bufferb;

    floatbuffer_t* buffertopa;
    floatbuffer_t* buffertopb;

    floatbuffer_t* buffercube;
    floatbuffer_t* buffertext;

    floatbuffer_t* buffertemp;
    floatbuffer_t* buffertoptemp;
};

extern buffers_t buffers;

void buffers_init(
    void);

void buffers_free(
    void);

void buffers_reset(
    void);

void buffers_upload(
    void);

void buffers_switcha(
    void);

void buffers_switchb(
    void);

void buffers_resetscene(
    void);

#endif

#if __INCLUDE_LEVEL__ == 0

#include "bus.c"

buffers_t buffers;

void buffers_init(
    void)
{
    buffers.buffera = floatbuffer_alloc();
    buffers.bufferb = floatbuffer_alloc();

    buffers.buffertopa = floatbuffer_alloc();
    buffers.buffertopb = floatbuffer_alloc();

    buffers.buffercube = floatbuffer_alloc();
    buffers.buffertext = floatbuffer_alloc();

    buffers.buffertemp    = floatbuffer_alloc();
    buffers.buffertoptemp = floatbuffer_alloc();
}

void buffers_free(
    void)
{
    REL(buffers.buffera);
    REL(buffers.bufferb);

    REL(buffers.buffertopa);
    REL(buffers.buffertopb);

    REL(buffers.buffercube);
    REL(buffers.buffertext);

    REL(buffers.buffertemp);
    REL(buffers.buffertoptemp);
}

void buffers_reset(
    void)
{
    floatbuffer_reset(buffers.buffera);
    floatbuffer_reset(buffers.bufferb);

    floatbuffer_reset(buffers.buffertopa);
    floatbuffer_reset(buffers.buffertopb);

    floatbuffer_reset(buffers.buffercube);
    floatbuffer_reset(buffers.buffertext);

    floatbuffer_reset(buffers.buffertemp);
    floatbuffer_reset(buffers.buffertoptemp);
}

void buffers_resetscene(
    void)
{
    floatbuffer_reset(buffers.buffera);
    floatbuffer_reset(buffers.bufferb);

    floatbuffer_reset(buffers.buffertopa);
    floatbuffer_reset(buffers.buffertopb);

    floatbuffer_reset(buffers.buffercube);
}

/* update buffers in renderer */

void buffers_uploadbuffer(
    floatbuffer_t* buffer,
    int            level)
{
    if (buffer->changed == 1)
    {

	renderdata_t data = {
	    level,
	    buffer};

	bus_notify(
	    "SCN",
	    "UPDBUFF",
	    &data);

	buffer->changed = 0;
    }
}

/* update projection matrixes in renderer */

void buffers_updatematrix(
    floatbuffer_t* buffer,
    int            level)
{

    renderdata_t data =
	{

	    level,
	    buffer

	};

    bus_notify(
	"SCN",
	"UPDPRJ",
	&data);
}

void buffers_upload(
    void)
{

    buffers_uploadbuffer(
	buffers.buffera,
	0);

    buffers_uploadbuffer(
	buffers.bufferb,
	1);

    buffers_uploadbuffer(
	buffers.buffertopa,
	2);

    buffers_uploadbuffer(
	buffers.buffertopb,
	3);

    buffers_uploadbuffer(
	buffers.buffercube,
	4);

    buffers_uploadbuffer(
	buffers.buffertext,
	5);

    buffers_updatematrix(
	buffers.buffera,
	0);

    buffers_updatematrix(
	buffers.bufferb,
	1);

    buffers_updatematrix(
	buffers.buffertopa,
	2);

    buffers_updatematrix(
	buffers.buffertopb,
	3);
}

void buffers_switcha(
    void)
{

    floatbuffer_t* buffer    = buffers.buffera;
    floatbuffer_t* buffertop = buffers.buffertopa;

    buffers.buffera    = buffers.buffertemp;
    buffers.buffertopa = buffers.buffertoptemp;

    buffers.buffertemp    = buffer;
    buffers.buffertoptemp = buffertop;

    floatbuffer_reset(buffer);
    floatbuffer_reset(buffertop);
}

void buffers_switchb(
    void)
{

    floatbuffer_t* buffer    = buffers.bufferb;
    floatbuffer_t* buffertop = buffers.buffertopb;

    buffers.bufferb    = buffers.buffertemp;
    buffers.buffertopb = buffers.buffertoptemp;

    buffers.buffertemp    = buffer;
    buffers.buffertoptemp = buffertop;

    floatbuffer_reset(buffer);
    floatbuffer_reset(buffertop);
}

#endif
