/********************************************************************************
* @Programmer: Tyler Raborn
* @Course: CS1566 
* @Assignment: Assignment 5 - RayTracer
* @Description: declaration of my Camera class
* @Dev Environment: g++ compiler on 64-bit Xubuntu Linux
* @GPU: nVidia GeForce GTX 670 2GB
*********************************************************************************/
#define SHAPE_CUBE 0
#define SHAPE_CYLINDER 1
#define SHAPE_SPHERE 2
#define SHAPE_TORUS 3
#define SHAPE_CONE 4

#include <deque>
#include <GL/glut.h>
#include "matrix.h"
#include "Vertex.h"
//#include "Shape.h"

#ifndef _CAM_H
#define _CAM_H

class Shape; //forward declaration

class Camera 
{

public:

	//constructors/destructors
	Camera();
	explicit Camera(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
	~Camera();

	//methods for Shape/Camera inter-class operations
	void synchronizedRotate(const GLdouble, Shape*);
	void synchronizedRotate(const GLdouble, std::deque<Shape*>);
	void synchronizedTranslate(const GLdouble, const GLdouble, const GLdouble, Shape*);
	void synchronizedTranslate(const GLdouble, const GLdouble, const GLdouble, std::deque<Shape*>);

	//public methods
	float** getCTM();
	void transform();
	void align();
	void scale(GLdouble, GLdouble, GLdouble, GLdouble);
	void scalePerspective(GLdouble, GLdouble, GLdouble, GLdouble);
	void print();

	//camera transformation:
	void translateCamera(GLdouble, GLdouble, GLdouble);
	void translateCameraLocalized(GLdouble, GLdouble, GLdouble);
	void rotateCameraLocalized(GLdouble, GLdouble, GLdouble, GLdouble);
	void transformReturn();
	void incrementHeight();
	void decrementHeight();
	void zoomIn();
	void zoomOut();
	void decreaseAR();
	void increaseAR();
	void increaseNCP();
	void decreaseNCP();
	void increaseFCP();
	void decreaseFCP();
	GLdouble getFar();
	GLdouble* getPosition();

	//"features"
	void jump(GLdouble);
	void crouch(GLdouble);


	bool rest;
	GLdouble dy;

private:

	std::deque<Shape> objectList;

	//private fields
	GLdouble w[3];
	GLdouble v[3];
	GLdouble u[3];
	GLdouble CTM[4][4];
	
	Vertex Pn;
	GLdouble pX, pY, pZ;
	GLdouble** curT;
	GLdouble lookPoint[3];
	GLdouble dOrthoNCP, dOrthoFCP;
	GLdouble dPerspNCP, dPerspFCP;
	GLdouble zoomAmt;
	GLdouble dAspect;

	GLdouble eyePoint[3];
	GLdouble lookVector[3];
	//GLdouble heightAngle[3];
	GLdouble heightAngle;
	GLdouble curAR;

	GLdouble tX, tY, tZ;
	bool falling;

	GLdouble far;



};

#endif


