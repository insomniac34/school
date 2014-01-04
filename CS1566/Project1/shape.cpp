/* @Programmer: Tyler Raborn
 * @Assignment: CS1566 Assignment 1
 * @Environment: Visual Studio 2012, Windows 8 Pro 64-bit, classic GLUT library installed directly into Visual Studio's Visual C/C++ lib/include directories.
 * @Description: This file provides the implementation of my Shape class.
 */


#define _USE_MATH_DEFINES
#include <math.h>
#include "Shape.h"
#include <algorithm>
#include <stdlib.h>
#include <GL/glut.h>
#define RADIANS M_PI / 180.0

//default Shape constructor
Shape::Shape()
{
	bounceUp = false;
}

//Shape constructor 
Shape::Shape(int shapeType, double vertices[2], int color[3]) //constructor for creating a stationary 2D shape
{
	f = 2; //resistance. The amount added/subtracted from the current size of Y-axis increments, to give the illusion of gravitational acceleration/resistance.
	dy = 0.0; //the change in the object's Y-position. Initially set to 0.
	dx = 1.0; //the change in the object's x position as it bounces. Stays at 1.0.

	rest = false; //a boolean for determining whether to animate movement or not.
	type = shapeType;

	std::copy(color, color+3, rgb); //use std copy algorithm to copy actual values from parameterized array into instance variable rgb[]
	std::copy(vertices, vertices+2, vertexArray);

	if (shapeType==0) numVertices=4;
	else if (shapeType==1) numVertices=1;
	else if (shapeType==2) numVertices=3; 
	
}

//handles motion & physics. Called whenever the shape needs to be updated; it's speed, vector and velocity are calculated relative to it's current position and the window height.
void Shape::update(double window_h)
{
	if (vertexArray[1]<10.0) dy*=-1.0; //reverse velocity when ground is hit.
	dy+=f; //apply resistance.
	vertexArray[0]+=dx; //update x pos
	vertexArray[1]-=dy; //update y pos
	if (vertexArray[1] < 0.0) vertexArray[1]=0.0; //correct any negative values resulting from incrementing past zero.
	printf("y = %g dy = %g\n", vertexArray[1], dy);
	if ((vertexArray[1]==0) && (abs(dy)<=10)) rest=true; //if the object settles on Y=0 w/ an increment less than 10, halt further updates. The ball/rectangle is at rest.
}

//This method creates a pseudocircle by generating a polygon from numerous triangles. 
void Shape::glCirclef(GLfloat originX, GLfloat originY, double window_w, double window_h) 
{
	originX = originX / window_w; 
	originY = originY / window_h; 
	GLfloat r = 20.0 / window_w; //circle radius adjusted for windowsize
	GLfloat initPos = 0; //first pieslice at angle 0 rads
	GLfloat endPos = (360*RADIANS); //drawing of pie slices ends at 360 degrees; forms a circle when complete
	glBegin(GL_TRIANGLES); //initiate triangle drawing mode; necessary for illusion of pie slices (technically triangles)

	//coordinates for the vector from the circle origin to point on circle line
	GLfloat X1 = originX + (r*cos(initPos));
	GLfloat Y1 = originY + (r*sin(initPos));

	//coordinates for second partition of pie slice
	GLfloat X0;
	GLfloat Y0;

	//for each interval of 1 rad, draw a triangle with vertices located at: 1.) the circle's origin, 2.) X1, 3.) Y1
	for (GLfloat i = RADIANS; i < endPos + RADIANS; i+=RADIANS) 
	{
		//swap vectors
		X0 = X1;
		Y0 = Y1;
		
		//make new vector
		X1 = originX + (r*cos(i)); //update x position of new vector rotation on unit circle
		Y1 = originY + (r*sin(i)); //update y position of new vector rotation on unit circle

		//draw the triangle based on the following defined vertices:
		glVertex2d(originX, originY);
		glVertex2d(X0, Y0);
		glVertex2d(X1, Y1);
	}
	glEnd(); //end drawing
}

//this method calls the appropriate animation method for each shape object.
void Shape::drawShape(double window_w, double window_h, double my_rect_h, double my_rect_w)
{
	if (type == 0) 
	{
		//printf("shape::drawShape DRAWING RECTANGLE\n");
		glColor3f(0.0, 0.0, 1.0);
		glRectf(vertexArray[0] / window_w, vertexArray[1] / window_h, vertexArray[0] / window_w + my_rect_w, vertexArray[1] / window_h + my_rect_h);
	}
	else
	{
		//printf("shape::drawShape DRAWING CIRCLE\n");
		glColor3f(1.0, 0.0, 0.0);
		glCirclef(vertexArray[0], vertexArray[1], window_w, window_h);
	}	
}