//written by Tyler Raborn

#include <vector>
#include "Vertex.h"

#ifndef FACE_H
#define FACE_H

class Face
{

private:
	void calcNormal();

public:

	//constructors:
	Face();
	Face(Vertex, Vertex, Vertex);
	//Face(Vertex, Vertex, Vertex, Vertex);
	Face::Face(Vertex* p0, Vertex* p1, Vertex* p2, Vertex* p3);
	~Face();

	//public fields:
	std::vector<Vertex*> faceArray; //holds the vertices corresponding to each Face.
	int size;
	Vertex* facepoint0;
	Vertex* facepoint1;
	Vertex* facepoint2;
	Vertex* facepoint3;
	Vertex normal; 

	//public methods:
	void print();
	void draw(int);
	Vertex getNormal();
	
	Vertex getFP0();
	Vertex getFP1();
	Vertex getFP2();
	Vertex getFP3();
};

#endif