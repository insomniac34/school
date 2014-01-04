/* @Programmer: Tyler Raborn
 * @Course: CS 1566 
 * @Assignment: Assignment 2 - 3D Tesselation
 * @Description: Header file for my Shape class for holding & maintaining data on 3D shapes.
 * @Dev Environment: Xubuntu Linux 64-bit, g++ 4.7.3
 * @GPU: nVidia GeForce GTX 670 2GB
 */

#define SHAPE_CUBE 0
#define SHAPE_CYLINDER 1
#define SHAPE_SPHERE 2
#define SHAPE_TORUS 3
#define SHAPE_CONE 4

#include <list>
#include <vector>
#include <GL/glut.h>
#include "Vertex.h"
#include "Face.h"

#ifndef _SHAPE_H
#define _SHAPE_H

class Shape
{
	
public:	
	Shape(); 
	explicit Shape(int, GLdouble, int, int, GLdouble); //This constructor initializes all necessary fields AND performs calculations on params to generate & store vertices
	~Shape();

	std::vector<std::vector<Vertex> > vertexArray;

	//accessors/mutators
	const int getType();
	const void draw(int, bool, const GLfloat*);

	//Vertex manipulation:
	void tessellate(); 
	void translate(GLfloat, GLfloat, GLfloat);
	void scale(GLfloat, GLfloat, GLfloat);
	void rotate(GLfloat, GLfloat, GLfloat, GLfloat);
	void revolve(GLfloat, GLfloat, GLfloat, GLfloat);
	void arbitraryRotate(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);

private:
	int rs, vs;
	int type; //0 = cube, 1 = cylinder, 2 = sphere, 3 = torus
	GLfloat* rgb; //array containing 3 rgb color values {red, green, blue}

	//axes
	Vertex position; //current center point
	Vertex curXAxis; //current X axis
	Vertex curAxis;  //current Y axis
	Vertex curZAxis; /// current Z axis

	//core data structures:
	std::vector<Vertex> normArray;
	std::vector<std::vector<Face> > faceArray;

	//private methods:
	const void drawCube(int, bool); //contains necessary calculations to draw a tessellated 3d cube
	const void drawCylinder(int, bool); //contains necessary calculations to draw a tessellated 3d cylinder
	const void drawSphere(int, bool); //contains necessary calculations to draw a tessellated 3d sphere
	const void drawTorus(int, bool); //contains necessary calculations to draw a tessellated 3d torus
	const void drawCone(int, bool); //contains necessary calculations to draw a tessellated 3d cone
};

#endif