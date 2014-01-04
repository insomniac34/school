//written by Tyler Raborn

#include <GL/glut.h>

#ifndef VERTEX_H
#define VERTEX_H

class Vertex
{

public:

	//public constructors
	Vertex();
	Vertex(GLdouble, GLdouble, GLdouble);
	explicit Vertex(GLdouble, GLdouble, GLdouble, GLdouble);
	Vertex(GLdouble*);
	GLdouble* normal;

	~Vertex();

	//public fields
	GLdouble x;
	GLdouble y;
	GLdouble z;
	GLdouble w;

	//public methods:
	void print();
	GLdouble* toArray(); //converts the vertex to a double array. Usually used for the convenience of my matrix functions.
	

private:
	//GLdouble* v;

};


#endif