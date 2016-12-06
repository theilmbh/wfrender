#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "wf_render.h"
#define WIDTH 300
#define HEIGHT 300

Display *dis;
int screen;
Window win;
GC gc;

void renderWireframe(obj* object, vec3 campos, double f)
{
    /* renders the object within the buffer */
    vec2 tra, trb, trc;
    triangle cur_tri;
    int i;

    for(i=0; i<object->nFaces; i++)
    {
        cur_tri = object->triangles[i]; 
	tra = project2D(cur_tri.a, campos, f);
        trb = project2D(cur_tri.b, campos, f);
        trc = project2D(cur_tri.c, campos, f);
        draw_line(&tra, &trb);
        draw_line(&trb, &trc);
	draw_line(&trc, &tra);	
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
    float t1x, t1y, t1z;
    float t2x, t2y, t2z;
    float t3x, t3y, t3z;
    FILE *fd = fopen(fname, "r");
    int i;
    obj *newobj = malloc(sizeof(obj));
    triangle *trilist = malloc(nTriangle*sizeof(triangle));
    triangle *newtriangle;
    newobj->nFaces = nTriangle;
    char objln[128];
 
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
    
    x1 = (int)round(pta->u * WIDTH);
    x2 = (int)round(ptb->u * WIDTH);
    y1 = (int)round(pta->v * HEIGHT);
    y2 = (int)round(ptb->v * HEIGHT);

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
void init_x()
{
        unsigned long black,white;

        dis=XOpenDisplay((char *)0);
        screen=DefaultScreen(dis);
        black=BlackPixel(dis,screen),
        white=WhitePixel(dis, screen);
        win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,
                300, 300, 5,black, white);
        XSetStandardProperties(dis,win,"Howdy","Hi",None,NULL,0,NULL);
        XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);
        gc=XCreateGC(dis, win, 0,0);
        XSetBackground(dis,gc,white);
        XSetForeground(dis,gc,black);
        XClearWindow(dis, win);
        XMapRaised(dis, win);
}

int main()
{

}

