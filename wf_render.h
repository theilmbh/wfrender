

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

    double ux;
    double uy;
    double uz;
    double theta;
}quat;

typedef struct {

    int nVerts;
    int nFaces;
    double *vertices;
    double *faces;
    triangle *triangles;
    vec3 xlat;
    quat orient;
}obj;

void renderWireframe(obj* object, vec3 campos, double f);
vec2 project2D(vec3 point, vec3 campos, double f);
obj *readObj(char *fname, int nTriangle, int nverts);
obj *readRaw(char *fname, int nTriangle);
void draw_line(vec2 *pta, vec2 *ptb);
void transformObj(obj* object, vec3 xlat, quat dorient);
vec3 transform3D(vec3 point, vec3 xlat, quat orient);
