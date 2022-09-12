#ifndef pixeltext_h
#define pixeltext_h

#include "zc_util2.c"
#include "zc_vec2.c"
#include "zc_vector.c"
#include <stdio.h>

v2_t   pixeltext_calcsize(char* label, float pixelsize);
vec_t* pixeltext_generate(char* label, float pixelsize);

#endif

#if __INCLUDE_LEVEL__ == 0

// returns pixeltext pattern

char* pixeltext_getpattern(char codepoint)
{
    char* pattern = NULL;
    switch (codepoint)
    {
	case 'A': pattern = "000 111 101 111 101 101\0"; break;
	case 'B': pattern = "000 110 101 110 101 110\0"; break;
	case 'C': pattern = "000 111 100 100 100 111\0"; break;
	case 'D': pattern = "000 110 101 101 101 110\0"; break;
	case 'E': pattern = "000 111 100 111 100 111\0"; break;
	case 'F': pattern = "000 111 100 111 100 100\0"; break;
	case 'G': pattern = "000 111 100 101 101 111\0"; break;
	case 'H': pattern = "000 101 101 111 101 101\0"; break;
	case 'I': pattern = "0 1 1 1 1 1\0"; break;
	case 'J': pattern = "000 001 001 001 101 111\0"; break;
	case 'K': pattern = "000 101 101 110 101 101\0"; break;
	case 'L': pattern = "000 100 100 100 100 111\0"; break;
	case 'M': pattern = "00000 11111 10101 10101 10101 10101\0"; break;
	case 'N': pattern = "000 111 101 101 101 101\0"; break;
	case 'O': pattern = "000 111 101 101 101 111\0"; break;
	case 'P': pattern = "000 111 101 111 100 100\0"; break;
	case 'Q': pattern = "000 111 101 101 101 111 001\0"; break;
	case 'R': pattern = "000 111 101 110 101 101\0"; break;
	case 'S': pattern = "000 111 100 111 001 111\0"; break;
	case 'T': pattern = "000 111 010 010 010 010\0"; break;
	case 'U': pattern = "000 101 101 101 101 111\0"; break;
	case 'V': pattern = "000 101 101 101 101 010\0"; break;
	case 'W': pattern = "00000 10101 10101 10101 10101 11111\0"; break;
	case 'X': pattern = "000 101 101 010 101 101\0"; break;
	case 'Y': pattern = "000 101 101 111 010 010\0"; break;
	case 'Z': pattern = "000 111 001 111 100 111\0"; break;
	case '0': pattern = "000 010 101 101 101 010\0"; break;
	case '1': pattern = "00 01 11 01 01 01\0"; break;
	case '2': pattern = "000 110 001 010 100 111\0"; break;
	case '3': pattern = "000 110 001 110 001 110\0"; break;
	case '4': pattern = "000 100 101 111 010 010\0"; break;
	case '5': pattern = "000 111 100 111 001 110\0"; break;
	case '6': pattern = "000 011 100 111 101 111\0"; break;
	case '7': pattern = "000 111 001 011 001 001\0"; break;
	case '8': pattern = "000 111 101 111 101 111\0"; break;
	case '9': pattern = "000 111 101 111 001 111\0"; break;
	case '.': pattern = "0 0 0 0 0 1\0"; break;
	case ',': pattern = "0 0 0 0 0 1 1\0"; break;
	case '!': pattern = "0 1 1 1 0 1\0"; break;
	case '?': pattern = "0000 0110 1001 0010 0000 0010\0"; break;
	case ':': pattern = "0 0 1 0 1 0\0"; break;
	case ' ': pattern = "0 0 0 0 0 0\0"; break;
	case '/': pattern = "000 001 001 010 100 100\0"; break;
	case '(': pattern = "01 10 10 10 10 10 01\0"; break;
	case ')': pattern = "10 01 01 01 01 01 10\0"; break;
	case '_': pattern = "000 000 000 000 000 111\0"; break;
	case '-': pattern = "000 000 000 111 000 000\0"; break;
	case '+': pattern = "000 000 010 111 010 000\0"; break;
	case 'l': pattern = "000 001 010 100 010 001\0"; break;
	case 'r': pattern = "000 100 010 001 010 100\0"; break;
	case 'u': pattern = "00000 00000 00100 01010 10001 00000\0"; break;
	case 'd': pattern = "00000 00000 10001 01010 00100 00000\0"; break;
	case '<': pattern = "00000 00100 01000 10000 01000 00100\0"; break;
	case '>': pattern = "00000 00100 00010 00001 00010 00100\0"; break;
    }
    return pattern;
}

// pre-calculates size of label

v2_t pixeltext_calcsize(char* label, float pixelsize)
{
    char* string = label;

    v2_t size = {0};

    while (*string != '\0')
    {
	char* pattern = pixeltext_getpattern(*string);

	if (pattern != NULL)
	{
	    int index  = 0;
	    int spaces = 0;

	    while (*pattern != '\0')
	    {
		if (*pattern == ' ') spaces++;
		else index++;

		pattern += 1;
	    }

	    int rows = spaces + 1;
	    int cols = index / rows;

	    if (size.y < rows * pixelsize) size.y = rows * pixelsize;

	    size.x += cols * pixelsize;
	    size.x += pixelsize; // space between two charaters
	}

	string += 1;
    }

    size.x -= pixelsize;
    size.y = 5 * pixelsize;

    return size;
}

/* generates label into a floatbuffer */

vec_t* pixeltext_generate(char* label, float pixelsize)
{
    vec_t* result = VNEW();

    char* string = label;

    v2_t trans = {0.0, pixelsize};

    while (*string != '\0')
    {
	char* pattern = pixeltext_getpattern(*string);

	if (pattern != NULL)
	{
	    int col = 0;
	    int row = 0;

	    while (*pattern != '\0')
	    {
		if (*pattern == ' ')
		{
		    row += 1;
		    col = 0;
		}
		else if (*pattern == '1')
		{
		    v2_t* coord = CAL(sizeof(v2_t), NULL, NULL);

		    coord->x = trans.x + pixelsize * col;
		    coord->y = trans.y - pixelsize * row;

		    VADD(result, coord);

		    REL(coord);
		}

		if (*pattern == '0' || *pattern == '1') col += 1;

		pattern += 1;
	    }

	    trans.x += (col + 1) * pixelsize;
	}

	string += 1;
    }

    return result;
}

#endif
