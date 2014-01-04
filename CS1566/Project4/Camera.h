/********************************************************************************
* @Programmer: Tyler Raborn
* @Course: CS1566 
* @Assignment: Assignment 4 
* @Description: declaration of my Camera class
* @Dev Environment: Xubuntu Linux 64-bit, g++ 4.7.3
* @GPU: nVidia GeForce GTX 670 2GB
*********************************************************************************/

#include <deque>
#include "GL/glut.h"
#include "matrix.h"
#include "Vertex.h"
#include "Shape.h"

#ifndef _CAM_H
#define _CAM_H

class Camera 
{

public:

	//constructors/destructors
	Camera();
	explicit Camera(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
	~Camera();

	//public methods
	const void transform();
	const void align();
	const void scale(GLdouble, GLdouble, GLdouble, GLdouble);
	const void scalePerspective(GLdouble, GLdouble, GLdouble, GLdouble);
	const void print();

	//camera transformation:
	void translateCamera(GLdouble, GLdouble, GLdouble);
	void translateCameraLocalized(GLdouble, GLdouble, GLdouble);
	void rotateCameraLocalized(GLdouble, GLdouble, GLdouble, GLdouble);

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

private:

	//private fields
	GLdouble w[3];
	GLdouble v[3];
	GLdouble u[3];
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


};

#endif


