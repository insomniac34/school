//written by Tyler Raborn

#include <cstdio>
#include <algorithm>
#include <GL/glut.h>
#include "Vertex.h"

Vertex::Vertex()
{

}

Vertex::Vertex(GLdouble c1, GLdouble c2, GLdouble c3)		
{
	x = c1;
	y = c2;
	z = c3;
	w = 1.0;
}

Vertex::Vertex(GLdouble* vector)
{
	GLdouble v0 = vector[0];
	GLdouble v1 = vector[1];
	GLdouble v2 = vector[2];

	x = v0;
	y = v1;
	z = v2;

	w=1.0;
}

void Vertex::print() //for testing
{
	printf("\nVertex: {%g, %g, %g, %g}\n", x, y, z, w);
}

GLdouble* Vertex::toArray() 
{
	GLdouble* ret = new GLdouble[4];

	ret[0] = x;
	ret[1] = y;
	ret[2] = z;
	ret[3] = w;

	return ret; //WARNING: RetVal is allocated on the HEAP!!! Callee variable deallocation required!
}

Vertex::~Vertex()
{

}