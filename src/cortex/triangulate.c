#ifndef triangulate_h
#define triangulate_h

#include "zc_vector.c"
#include <stdio.h>

char  triangulate_process(vec_t* contour, vec_t* result);
float triangulate_area(vec_t* contour);
char  triangulate_inside_triangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py);
char  triangulate_snip(vec_t* contour, int u, int v, int w, int n, int* V);

#endif

#if __INCLUDE_LEVEL__ == 0

#include "zc_util2.c"
#include "zc_vec2.c"

#include "zc_memory.c"

#define EPSILON 0.0000000001f

/* calculates area of contour */

float triangulate_area(vec_t* contour)
{
    int n = contour->length;

    float A = 0.0f;

    for (int p = n - 1, q = 0; q < n; p = q++)
    {
	v2_t* pv = contour->data[p];
	v2_t* qv = contour->data[q];
	A += pv->x * qv->y - qv->x * pv->y;
    }
    return A * 0.5f;
}

/* InsideTriangle decides if a point P is Inside of the triangle
   defined by A, B, C. */

char triangulate_inside_triangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py)
{
    float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
    float cCROSSap, bCROSScp, aCROSSbp;

    ax  = Cx - Bx;
    ay  = Cy - By;
    bx  = Ax - Cx;
    by  = Ay - Cy;
    cx  = Bx - Ax;
    cy  = By - Ay;
    apx = Px - Ax;
    apy = Py - Ay;
    bpx = Px - Bx;
    bpy = Py - By;
    cpx = Px - Cx;
    cpy = Py - Cy;

    aCROSSbp = ax * bpy - ay * bpx;
    cCROSSap = cx * apy - cy * apx;
    bCROSScp = bx * cpy - by * cpx;

    return ((aCROSSbp > 0.0f) && (bCROSScp > 0.0f) && (cCROSSap > 0.0f));
};

/* snips contour */

char triangulate_snip(vec_t* contour, int u, int v, int w, int n, int* V)
{
    int   p;
    float Ax, Ay, Bx, By, Cx, Cy, Px, Py;

    v2_t* uv = contour->data[V[u]];
    v2_t* vv = contour->data[V[v]];
    v2_t* wv = contour->data[V[w]];

    Ax = uv->x;
    Ay = uv->y;

    Bx = vv->x;
    By = vv->y;

    Cx = wv->x;
    Cy = wv->y;

    if (EPSILON > (((Bx - Ax) * (Cy - Ay)) - ((By - Ay) * (Cx - Ax))))
    {
	return 0;
    }

    for (p = 0; p < n; p++)
    {
	v2_t* pv = contour->data[V[p]];

	if ((p == u) || (p == v) || (p == w)) continue;
	Px = pv->x;
	Py = pv->y;
	if (triangulate_inside_triangle(Ax, Ay, Bx, By, Cx, Cy, Px, Py) == 1)
	{
	    return 0;
	}
    }
    return 1;
}

/* processes contour, returns triangles in result vector */

char triangulate_process(vec_t* contour, vec_t* result)
{
    /* allocate and initialize list of Vertices in polygon */

    int n = contour->length;
    if (n < 3) return 0;

    int* V = CAL(sizeof(int) * n, NULL, NULL);

    /* we want a counter-clockwise polygon in V */

    if (0.0f < triangulate_area(contour))
	for (int v = 0; v < n; v++) V[v] = v;
    else
	for (int v = 0; v < n; v++) V[v] = (n - 1) - v;

    int nv = n;

    /*  remove nv-2 Vertices, creating 1 triangle every time */
    int count = 2 * nv; /* error detection */

    for (int m = 0, v = nv - 1; nv > 2;)
    {
	/* if we loop, it is probably a non-simple polygon */
	if (0 >= (count--))
	{
	    //** Triangulate: ERROR - probably bad polygon!
	    REL(V);
	    return 0;
	}

	/* three consecutive vertices in current polygon, <u,v,w> */
	int u = v;
	if (nv <= u) u = 0; /* previous */
	v = u + 1;
	if (nv <= v) v = 0; /* new v    */
	int w = v + 1;
	if (nv <= w) w = 0; /* next     */

	if (triangulate_snip(contour, u, v, w, nv, V) == 1)
	{
	    int a, b, c, s, t;

	    /* true names of the vertices */
	    a = V[u];
	    b = V[v];
	    c = V[w];

	    /* output Triangle */
	    VADD(result, contour->data[a]);
	    VADD(result, contour->data[b]);
	    VADD(result, contour->data[c]);

	    m++;

	    /* remove v from remaining polygon */
	    for (s = v, t = v + 1; t < nv; s++, t++) V[s] = V[t];
	    nv--;

	    /* resest error detection counter */
	    count = 2 * nv;
	}
    }

    REL(V);

    return 1;
}

#endif
