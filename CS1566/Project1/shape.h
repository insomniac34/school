/* @Programmer: Tyler Raborn
 * @Assignment: CS1566 Assignment 1
 * @Environment: Visual Studio 2012, Windows 8 Pro 64-bit, classic GLUT library installed directly into Visual Studio's Visual C/C++ lib/include directories.
 * @Description: This .h file provides the prototype for my Shape class
 */

#include <stdlib.h>
#include <GL/glut.h>
#ifndef _SHAPE_H
#define _DATE_H

class Shape
{

private:

	//private fields:
	double dx;
	double f;
	double dy; //value for determining the incremental update size of the shape's Y position.
	bool bounceUp;
	double bounceMax; //decrements as bouncing object looses energy.
	double startPos; //where the shape is "dropepd" from
	double prevHeight;

	//private methods:
	void glCirclef(GLfloat, GLfloat, double, double); //draws a circle. My custom replacement for the mythical glCirclef() function.

public:
	
	//public constructors:
	Shape();
	Shape(int, double[], int[]);

	//public fields:
	bool rest;
	int type; //0 = rectangle, 1 = circle, 2 = triangle
	int numVertices; //stores total number of vertices in the 2D object.
	double vertexArray[2]; //a dynamic array that stores each vertex's (x, y)-coordinates in alternating successive indices
	int rgb[3]; //an array that contains the 3 rgb values s.t. rgb = {r, g, b}

	//public methods:
	void update(double);
	void drawShape(double, double, double, double);


};

#endif
