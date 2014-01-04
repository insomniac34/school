
#ifndef _RAY_TRACE_H_
#define _RAY_TRACE_H_

/*
#define _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG
*/

#include <cstdio>
#include <deque>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <thread>

// platform specific includes
#ifdef __APPLE__
 #include <GLUT/glut.h>
 #include <OpenGL/gl.h>
 #include <OpenGL/glu.h>
#else
 #include <GL/glut.h>
 #include <GL/gl.h>
 #include <GL/glu.h>
#endif

#include "vec3.h"
#include "matrix.h"
#include "Vertex.h"
#include "Light.h"
//#include "Camera.h"


#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

#define SHAPE_CUBE 1 //object id
#define SHAPE_HOUSE 2 //object id
#define SHAPE_SPHERE 3 //object id
#define SHAPE_CYLINDER 4 //object id
#define SHAPE_CONE 5 //object id
#define SHAPE_TORUS 6 //object id

typedef struct color_s
{
	float r, g, b, a;
} color_t;

class Shape //DOES NOT CONTAIN VERTICES; rather, a structure for maintaining how the shape has been transformed via its CTM.
{

public:
	Shape();
	Shape(GLdouble[10], GLdouble[13], int);
	~Shape();

	void rotate(GLdouble, GLdouble, GLdouble, GLdouble);
	void translate(GLdouble, GLdouble, GLdouble);
	void scale(GLdouble, GLdouble, GLdouble);

	int getType();
	GLdouble** getCTM();
	GLdouble** getCTMdir();
	GLdouble** getCTMinverse();

	//LIGHTING:
	GLdouble shiny;
	GLdouble emission[4];
	GLdouble amb[4];
	GLdouble diff[4];
	GLdouble spec[4];

private:

	GLdouble CTM[4][4];
	GLdouble CTMdir[4][4];
	GLdouble CTMinverse[4][4];
	int type;

};

std::deque<Shape> shapeList;
std::deque<Light> lightList;
//Camera myCamera;


extern GLuint textureId;
extern color_t screen[WINDOW_WIDTH * WINDOW_HEIGHT];

void plotPixel(color_t *screen, int x, int y, float r, float g, float b, float a);

#endif
