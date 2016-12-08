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
    vec3 tra1, trb1, trc1;
    triangle cur_tri;
    int i;

    for(i=0; i<object->nFaces; i++)
    {
        cur_tri = object->triangles[i]; 
        tra1 = transform3D(cur_tri.a, object->xlat, object->orient);
	trb1 = transform3D(cur_tri.b, object->xlat, object->orient);
        trc1 = transform3D(cur_tri.c, object->xlat, object->orient);

        tra = project2D(tra1, campos, f);
        trb = project2D(trb1, campos, f);
        trc = project2D(trc1, campos, f);
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

vec3 hamiltonProduct(vec3 point, quat orient)
{

   double c = cos(orient.theta / 2);
   double s = sin(orient.theta / 2);

   vec3 res;
   res.x = point.x*c + point.y*orient.uz*s - point.z*orient.uy*s;
   res.y = -point.x*orient.uz*s + point.y*c + point.z*orient.ux*s;
   res.z = point.x*orient.uy*s - point.y*orient.ux*s + point.z*c;

   return res;
}

vec3 transform3D(vec3 point, vec3 xlat, quat orient)
{

   vec3 xlated;
   xlated = hamiltonProduct(point, orient);
   xlated.x = xlated.x + xlat.x;
   xlated.y = xlated.y + xlat.y;
   xlated.z = xlated.z + xlat.z;

   return xlated;
}

void transformObj(obj* object, vec3 xlat, quat dorient)
{
    object->xlat = xlat;
    object->orient = dorient;
}

obj *readRaw(char *fname, int nTriangle)
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

obj *readObj(char *fname, int nTriangle, int nverts)
{
    /* Reads the object file and produces an obj struct */
    float t1x, t1y, t1z;
    int v1, v2, v3;
    FILE *fd = fopen(fname, "r");
    int i;
    char *v;
    obj *newobj = malloc(sizeof(obj));
    triangle *trilist = malloc(nTriangle*sizeof(triangle));
    vec3 *vertlist = malloc(nverts*sizeof(vec3)); 
    triangle *newtriangle;
    vec3 *newvert;
    newobj->nFaces = nTriangle;
    char objln[128];
    vec3 xlat = {0.0, 0.0, 0.0};
    quat orient = {0.0, 0.0, 0.0, 0.0};
 
    for(i=0; i<nverts; i++)
    {

	fgets(objln, sizeof(objln), fd);
    
	sscanf(objln, "%s %f %f %f", v, &t1x, &t1y, &t1z);
	newvert = &vertlist[i]; 
	newvert->x = t1x;
        newvert->y = t1y;
        newvert->z = t1z;
    }
    fgets(objln, sizeof(objln), fd); 
    for(i=0; i<nTriangle; i++)
    {
          fgets(objln, sizeof(objln), fd);
          sscanf(objln, "%s %d %d %d", v, &v1, &v2, &v3);
          newtriangle = &trilist[i];
          newtriangle->a = vertlist[v1];
          newtriangle->b = vertlist[v2];
          newtriangle->c = vertlist[v3];
    }
    newobj->triangles = trilist;
    newobj->xlat = xlat;
    newobj->orient = orient;
    return newobj;
}
void draw_line(vec2 *pta, vec2 *ptb)
{
    /* draws a line between two points in the buffer */
    int x1, x2, y1, y2;
    XWindowAttributes *wattr = malloc(sizeof(XWindowAttributes));
    
    XGetWindowAttributes(dis, win, wattr);
    int w = wattr->width;
    int h = wattr->height;    
    x1 = (int)floor(pta->u * w/2 + w/2);
    x2 = (int)floor(ptb->u * w/2 + w/2);
    y1 = (int)floor(pta->v * h/2 + h/2);
    y2 = (int)floor(ptb->v * h/2 + h/2);
/*    printf("%d %d %d %d \n", x1, y1, x2, y2); */
    XDrawLine(dis, win, gc, x1, y1, x2, y2);
}

/*void init_vga()
{
    vga_init();
    vga_setmode(G320x200x256);
    gl_setcontextvga(G320x200x256);
    physicalscreen = gl_allocatecontext();
    gl_getcontext(physicalscreen);
    ne(dis,win,gc, x1,y1, x2,y2); gl_setcontextvgavirtual(G320x200x256);
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
void redraw()
{
    XClearWindow(dis, win);
}

int main()
{
    char *fname = "./teapot.obj";
    int nTriangle = 6320;
    int nverts = 3644;
    obj *newobj = readObj(fname, nTriangle, nverts);   
    int i;
    double f = 0.5; 
    vec3 campos = {0.0, 0.0, -4.0};
    XEvent event;
    /*for(i=0; i<nTriangle; i++)
    {
      printf("%f\n", newobj->triangles[i].b.y); 
    }*/

    init_x();
    while(1)
    {
         XNextEvent(dis, &event);
         renderWireframe(newobj, campos, f);

         if(event.type==Expose && event.xexpose.count==0)
         {
             redraw();
         }
    }
}

