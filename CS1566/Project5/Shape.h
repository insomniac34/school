/* @Programmer: Tyler Raborn
 * @Course: CS 1566 
 * @Assignment: Assignment 5 - Raytracer
 * @Description: Header file for my Shape class for holding & maintaining data on 3D shapes.
 * @Dev Environment: Ubuntu Linux 13.10 (amd64), g++ 4.7
 * @GPU: nVidia GeForce GTX 670 2GB
 */

#define SHAPE_CUBE 0
#define SHAPE_CYLINDER 1
#define SHAPE_SPHERE 2
#define SHAPE_TORUS 3
#define SHAPE_CONE 4

#include <deque>
#include <vector>
#include <GL/glut.h>
#include "Vertex.h"
#include "Face.h"
#include "Camera.h"
//#include "RayTracer.h"

#ifndef _SHAPE_H
#define _SHAPE_H

class Shape
{
	
public:	
	Shape(); 
	explicit Shape(int, GLdouble, int, int, GLdouble, const GLfloat*); //This constructor initializes all necessary fields AND performs calculations on params to generate & store vertices
	explicit Shape(Shape*); //copy constructor
	~Shape();

	//accessors/mutators
	const int getType();
	float **getCTM();
	virtual void draw(int, bool, const GLfloat*);
	Vertex *getPosition();

	//methods for vertex manipulation:
	void tessellate(); 
	void translate(GLfloat, GLfloat, GLfloat);
	void scale(GLfloat, GLfloat, GLfloat);
	void rotate(GLfloat, GLfloat, GLfloat, GLfloat);
	void revolve(GLfloat, GLfloat, GLfloat, GLfloat);
	void arbitraryRotate(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);

	//methods for Camera/Object inter-class operations:
	friend void Camera::synchronizedRotate(const GLdouble, Shape*);
	friend void Camera::synchronizedRotate(const GLdouble, std::deque<Shape*>);
	friend void Camera::synchronizedTranslate(const GLdouble, const GLdouble, const GLdouble, Shape*);
	friend void Camera::synchronizedTranslate(const GLdouble, const GLdouble, const GLdouble, std::deque<Shape*>);

	//methods for Shape/RayTracer inter-class operations:
	//friend void RayTracer::fire(std::deque<Shape>*, GLdouble, GLdouble);

	//ray tracing methods for  project 5
	void paintXAxis(); //draw a ray originating at this->position and extending to edge of world
	GLdouble* isHit(Vertex, Vertex); 
	bool* fireRay(std::deque<Shape>*, std::deque<Shape>*);
	//friend RayTracer::RayTracer(Shape*);

	//methods for linking objects
	void link(Shape*);
	void link(std::deque<Shape*>);

	//"extra features":
	void jump();

private:

	int rs, vs;
	int type; //0 = cube, 1 = cylinder, 2 = sphere, 3 = torus
	GLfloat rgb[3]; //array containing 3 rgb color values {red, green, blue}
	GLdouble CTM[4][4]; //Cumulative Transformation Matrix
	GLdouble CTMinverse[4][4];
	GLdouble CTMtrans[4][4];
	GLdouble CTMdir_inverse[4][4]; //for direction

	//axes
	Vertex position; //current center point
	Vertex curXAxis; //current X axis
	Vertex curAxis;  //current Y axis
	Vertex curZAxis; /// current Z axis

	//core data structures:
	std::vector<std::vector<Vertex> > vertexArray;
	std::vector<Vertex> normArray;
	std::vector<std::vector<Face> > quadArray;

	GLdouble dy;
	bool rest;

	//private methods:
	void drawCube(int, bool); //contains necessary calculations to draw a tessellated 3d cube
	void drawCylinder(int, bool); //contains necessary calculations to draw a tessellated 3d cylinder
	void drawSphere(int, bool); //contains necessary calculations to draw a tessellated 3d sphere
	void drawTorus(int, bool); //contains necessary calculations to draw a tessellated 3d torus
	void drawCone(int, bool); //contains necessary calculations to draw a tessellated 3d cone
};

#endif