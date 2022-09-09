#ifndef mtbmp_h
#define mtbmp_h

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _mtbmp_t mtbmp_t;
struct _mtbmp_t
{
    int width;
    int height;

    uint8_t* bytes;
    uint32_t bytes_length;
};

mtbmp_t* mtbmp_alloc(int the_width, int the_height);
void     mtbmp_dealloc(void* the_bitmap);
mtbmp_t* mtbmp_alloc_from_bitmap(mtbmp_t* the_bitmap);
mtbmp_t* mtbmp_alloc_from_grayscale(int the_width, int the_height, uint32_t backcolor, uint32_t fontcolor, unsigned char* the_bitmap);
mtbmp_t* mtbmp_fill_with_color(mtbmp_t* bitmap, int the_sx, int the_sy, int the_ex, int the_ey, uint32_t color);
void     mtbmp_replace_from(mtbmp_t* the_base, mtbmp_t* the_bitmap, int the_x, int the_y);
void     mtbmp_blend_from(mtbmp_t* the_base, mtbmp_t* the_bitmap, int the_x, int the_y);

#endif

#if __INCLUDE_LEVEL__ == 0

#include "../core/mtmem.c"
#include <string.h>

/* creates empty bitmap with width and height */

mtbmp_t* mtbmp_alloc(int the_width, int the_height)
{
    mtbmp_t* bitmap = mtmem_calloc(sizeof(mtbmp_t), mtbmp_dealloc);

    bitmap->width  = the_width;
    bitmap->height = the_height;

    bitmap->bytes_length = 4 * the_width * the_height;
    bitmap->bytes        = mtmem_calloc(bitmap->bytes_length * sizeof(unsigned char), NULL);

    return bitmap;
}

/* dealloc */

void mtbmp_dealloc(void* pointer)
{
    mtbmp_t* the_bitmap = pointer;
    if (the_bitmap->bytes != NULL) mtmem_release(the_bitmap->bytes);
}

/* clone bitmap */

mtbmp_t* mtbmp_alloc_from_bitmap(mtbmp_t* the_bitmap)
{
    mtbmp_t* bitmap = mtbmp_alloc(the_bitmap->width, the_bitmap->height);

    memcpy(bitmap->bytes, the_bitmap->bytes, the_bitmap->bytes_length);

    return bitmap;
}

/* creates bitmap from grayscale bitmap with given colors */

mtbmp_t* mtbmp_alloc_from_grayscale(int the_width, int the_height, uint32_t backcolor, uint32_t fontcolor, unsigned char* the_bitmap)
{
    if (the_width == 0 || the_height == 0) return NULL;

    float br = (float) (backcolor >> 24 & 0xFF);
    float bg = (float) (backcolor >> 16 & 0xFF);
    float bb = (float) (backcolor >> 8 & 0xFF);
    float ba = (float) (backcolor & 0xFF);

    float fr = (float) (fontcolor >> 24 & 0xFF);
    float fg = (float) (fontcolor >> 16 & 0xFF);
    float fb = (float) (fontcolor >> 8 & 0xFF);
    float fa = (float) (fontcolor & 0xFF);

    mtbmp_t* bitmap = mtbmp_alloc(the_width, the_height);

    for (int index = 0; index < the_width * the_height; index++)
    {
	float ratio                  = (float) the_bitmap[index] / 255.0;
	bitmap->bytes[index * 4]     = (int) (fr * ratio + br * (1.0 - ratio));
	bitmap->bytes[index * 4 + 1] = (int) (fg * ratio + bg * (1.0 - ratio));
	bitmap->bytes[index * 4 + 2] = (int) (fb * ratio + bb * (1.0 - ratio));
	bitmap->bytes[index * 4 + 3] = (int) (fa * ratio + ba * (1.0 - ratio));
    }

    return bitmap;
}

/* fills bitmap with color */

mtbmp_t* mtbmp_fill_with_color(mtbmp_t* bitmap, int the_sx, int the_sy, int the_ex, int the_ey, uint32_t color)
{
    if (the_ex < the_sx) return bitmap;
    if (the_ey < the_sy) return bitmap;
    if (the_sx < 0) the_sx = 0;
    if (the_sy < 0) the_sy = 0;
    if (the_ex >= bitmap->width) the_ex = bitmap->width;
    if (the_ey >= bitmap->height) the_ey = bitmap->height;

    int r = color >> 24 & 0xFF;
    int g = color >> 16 & 0xFF;
    int b = color >> 8 & 0xFF;
    int a = color & 0xFF;

    for (int y = the_sy; y < the_ey; y++)
    {
	for (int x = the_sx; x < the_ex; x++)
	{
	    int position = (y * bitmap->width + x) * 4;

	    bitmap->bytes[position]     = r;
	    bitmap->bytes[position + 1] = g;
	    bitmap->bytes[position + 2] = b;
	    bitmap->bytes[position + 3] = a;
	}
    }

    return bitmap;
}

/* replaces a region of the bitmap with given bitmap */

void mtbmp_replace_from(mtbmp_t* the_base, mtbmp_t* the_bitmap, int the_x, int the_y)
{
    if (the_x < 0) the_x = 0;
    if (the_y < 0) the_y = 0;

    int width  = the_bitmap->width;
    int height = the_bitmap->height;

    if (the_x + width > the_base->width) return;   // width = the_base->width - the_x;
    if (the_y + height > the_base->height) return; // height = the_base->height - the_y;

    if (width <= 0 || height <= 0) return;

    for (int y = the_y; y < the_y + the_bitmap->height; y++)
    {
	int dstIndex = (y * the_base->width + the_x) * 4;
	int srcIndex = (y - the_y) * the_bitmap->width * 4;
	memcpy(the_base->bytes + dstIndex, the_bitmap->bytes + srcIndex, width * 4);
    }
}

/* blends a bitmap on bitmap src */

void mtbmp_blend_from(mtbmp_t* the_base, mtbmp_t* the_bitmap, int the_x, int the_y)
{
    mtbmp_t* bitmap = the_base;

    int   x, y, srcIndex, dstIndex;
    float srcR, srcG, srcB, srcA, dstR, dstG, dstB, dstA, outA, outR, outG, outB;

    int bx = the_x + the_bitmap->width;
    if (bx > the_base->width) bx = the_base->width;
    int by = the_y + the_bitmap->height;
    if (by > the_base->height) by = the_base->height;

    uint8_t* srcbytes = the_bitmap->bytes;
    uint8_t* dstbytes = bitmap->bytes;

    for (y = the_y; y < by; y++)
    {
	for (x = the_x; x < bx; x++)
	{
	    srcIndex = ((y - the_y) * the_bitmap->width + (x - the_x)) * 4;
	    dstIndex = (y * bitmap->width + x) * 4;

	    srcR = (float) srcbytes[srcIndex] / 255.0;
	    srcG = (float) srcbytes[srcIndex + 1] / 255.0;
	    srcB = (float) srcbytes[srcIndex + 2] / 255.0;
	    srcA = (float) srcbytes[srcIndex + 3] / 255.0;

	    dstR = (float) dstbytes[dstIndex] / 255.0;
	    dstG = (float) dstbytes[dstIndex + 1] / 255.0;
	    dstB = (float) dstbytes[dstIndex + 2] / 255.0;
	    dstA = (float) dstbytes[dstIndex + 3] / 255.0;

	    outA = srcA + dstA * (1 - srcA);

	    outR = (srcR * srcA + dstR * dstA * (1 - srcA)) / outA;
	    outG = (srcG * srcA + dstG * dstA * (1 - srcA)) / outA;
	    outB = (srcB * srcA + dstB * dstA * (1 - srcA)) / outA;

	    dstbytes[dstIndex]     = (int) (outR * 255.0);
	    dstbytes[dstIndex + 1] = (int) (outG * 255.0);
	    dstbytes[dstIndex + 2] = (int) (outB * 255.0);
	    dstbytes[dstIndex + 3] = (int) (outA * 255.0);
	}
    }
}

#endif
