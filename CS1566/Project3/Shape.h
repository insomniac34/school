/* @Programmer: Tyler Raborn
 * @Course: CS 1566 
 * @Assignment: Assignment 2 - 3D Tesselation
 * @Description: Header file for my Shape class for holding & maintaining data on 3D shapes.
 * @Dev Environment: Microsoft Visual Studio 2012, Windows 8 Pro 64-bit
 * @GPU: nVidia GeForce GTX 670 2GB
 */

#include <list>
#include <vector>
#include <GL/glut.h>
#include "Vertex.h"
#include "Face.h"

#define SHAPE_CUBE 0
#define SHAPE_CYLINDER 1
#define SHAPE_SPHERE 2
#define SHAPE_TORUS 3
#define SHAPE_CONE 4

#ifndef _SHAPE_H
#define _SHAPE_H

class Shape
{


private:

	//private fields:
	int type; //0 = cube, 1 = cylinder, 2 = sphere, 3 = torus
	int vertexCount; //total number of vertices contained within the object; dependent on object type
	GLdouble rgb[3]; //array containing 3 rgb color values {red, green, blue}
	GLdouble radius;
	int dim;
	int radialSlices; //# of 2d circles to make up cylinder
	int verticalSlices;
	int rs, vs;
	//GLdouble* axis; //first point defining the object's axis
	//Vertex a2; //second point defining the object's axis

	Vertex curXAxis; //current X axis
	Vertex curAxis;  //current Y axis
	Vertex curZAxis; /// current Z axis

	Vertex position;

	GLdouble cubeVertices[8][4];
	GLdouble cubeNormals[8][3];

	


	//private methods:
	void drawCube(GLdouble[][4], int, bool); //contains necessary calculations to draw a tesselated 3d cube
	void drawCylinder(int, bool); //contains necessary calculations to draw a tesselated 3d cylinder
	void drawSphere(int, bool); //contains necessary calculations to draw a tesselated 3d sphere
	void drawTorus(int, bool); //contains necessary calculations to draw a tesselated 3d torus
	void drawCone(int, bool);

	//core data structures:
	std::vector<std::vector<Vertex> > vertexArray;
	std::vector<Vertex> normArray;
	std::vector<std::vector<Face> > faceArray;
	
	
	

public:	

	//constructors:
	Shape(); //default/empty constructor
	Shape(int, GLdouble, int, int, GLdouble); //This constructor initializes all necessary fields AND performs calculations on params to generate & store vertices
	~Shape();

	//public fields:

	//public methods:
	int getType();
	void draw(int rs, int vs, int crt_mode, bool normalFlag);
	void tesselate(); 
	void translate(GLfloat, GLfloat, GLfloat);
	void scale(GLfloat, GLfloat, GLfloat);
	void rotate(GLfloat, GLfloat, GLfloat, GLfloat);
	void revolve(GLfloat, GLfloat, GLfloat, GLfloat);
	void align(Vertex, Vertex, Vertex); //uses Rrot matrix, composed of the three passed-in vectors
	void arbitraryRotate(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);



	

protected:

	//protected fields:

	//protected methods:
	//void draw_quad(GLfloat[4], int, int, int, int);
	//void glCirclef(GLfloat, GLfloat);
	


};

#endif