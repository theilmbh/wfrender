#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>

#include "wf_render.h"

GraphicsContext *virtualscreen;
GraphicsContext *physicalscreen;

typedef struct {
    double x;
    double y;
    double z;
}vec3;

typedef struct {
    double u;
    double v;
}vec2;

typedef struct {
    vec3 a;
    vec3 b;
    vec3 c;
}triangle;

typedef struct {

    int nVerts;
    int nFaces;
    double *vertices;
    double *faces;
    triangle *triangles;
}obj;

void renderWireframe(obj* object, vec3 campos, double f)
{
    /* renders the object within the buffer */
    vec2 tra, trb, trc;
    triangle *cur_tri;
    int i;

    for(i=0; i<obj.nFaces; i++)
    {
        cur_tri = obj->triangles[i]; 
	tra = project2D(cur_tri->a, campos, f);
        trb = project2D(cur_tri->b, campos, f);
        trc = project2D(cur_tri->c, campos, f);
        draw_line(&tra, &trb, buffer);
        draw_line(&trb, &trc, buffer);
	draw_line(&trc, &tra, buffer);	
    }
}

vec2 project2D(vec3 point, vec3 campos, double f)
{
    /* Projects a 3D point to a 2d plane */
    vec2 res;
    double dz = point.z - campos.z;
    res.u = (point.x-campos.x)*(f/dz);
    res.v = (point.y-campos.y)*(f/dz);
    return res;
}

obj *readObj(char *fname, int nTriangle)
{
    /* Reads the object file and produces an obj struct */
    double t1x, t1y, t1z;
    double t2x, t2y, t2z;
    double t3x, t3y, t3z;
    int fd = fopen(fname, 'r');
    int i;
    obj *newobj = malloc(sizeof(obj));
    triangle *trilist = malloc(nTriangle*sizeof(triangle));
    triangle *newtriangle;
    newobj->nFaces = nTriangle;
  
    for(i=0; i<nTriangle; i++)
    {

	fgets(objln, sizeof(objln), fd);
    
	sscanf(objln, "%f %f %f %f %f %f %f %f %f", &t1x, &t1y, &t1z,
		                                    &t2x, &t2y, &t2z,
						    &t3x, &t3y, &t3z);
	newtriangle = &trilist[i]; 
	newtriangle->a.x = t1x;
        newtriangle->a.y = t1y;
        newtriangle->a.z = t1z;
        newtriangle->b.x = t2x;
        newtriangle->b.y = t2y;
        newtriangle->b.z = t2z;
        newtriangle->c.x = t3x;
        newtriangle->c.y = t3y;
        newtriangle->c.z = t3z;
    }
    newobj->triangles = trilist;
    return newobj;
}

void draw_line(vec2 *pta, vec2 *ptb)
{
    /* draws a line between two points in the buffer */
    int x1, x2, y1, y2;
    gl_setcontext(virtualscreen);
    
    x1 = (int)round(pta->u * WIDTH);
    x2 = (int)round(ptb->u * WIDTH);
    y1 = (int)round(pta->v * HEIGHT);
    y2 = (int)round(ptb->v * HEIGHT);

    gl_line(x1, y1, x2, y2, 2);
}

/*void init_vga()
{
    vga_init();
    vga_setmode(G320x200x256);
    gl_setcontextvga(G320x200x256);
    physicalscreen = gl_allocatecontext();
    gl_getcontext(physicalscreen);
    gl_setcontextvgavirtual(G320x200x256);
    virtualscreen = gl_allocatecontext();
    gl_getcontext(virtualscreen);

}*/

int main()
{
   init_vga();


