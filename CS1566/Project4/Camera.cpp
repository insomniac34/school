/********************************************************************************
* @Programmer: Tyler Raborn
* @Course: CS1566 
* @Assignment: Assignment 4 
* @Description: Method & Constructor definitions for my Camera class
* @Dev Environment: Xubuntu Linux 64-bit, g++ 4.7.3
* @GPU: nVidia GeForce GTX 670 2GB
*********************************************************************************/

//DEFINITIONS:
#define _USE_MATH_DEFINES

//IMPORTS:

#include <cstdio>
#include <cmath>
#include <vector>
#include "Camera.h"

#define RADIANS 180.0/M_PI


//GLOBALS:
static const float Origin[3] = {0.0, 0.0, 0.0};
static float dH = 1.0;


/* QUOTES:
* @ 
* @ "it is Sacrilage to defile thine Defaultest of Constructors" Stroustrup, 10:5
*/

Camera::Camera()
{
	//it is Sacrilage to defile thine Defaultest of Constructors
}

Camera::Camera(GLdouble posx, GLdouble posy, GLdouble posz, GLdouble lookAtx, GLdouble lookAty, GLdouble lookAtz, GLdouble upx, GLdouble upy, GLdouble upz, GLdouble camX, GLdouble camY, GLdouble camZ)
{
	dPerspNCP=0.0;
	dOrthoNCP=0.0;
	dPerspFCP=0.0;
	dOrthoFCP=0.0;
	dH=0.0;
	dAspect=0.0;
	zoomAmt=0.0;
	upy = 18.0;

	//v = new GLdouble[3];
	//w = new GLdouble[3];
	pX = posx;
	pY = posy;
	pZ = posz;

	this->lookPoint[0]=lookAtx;
	this->lookPoint[1]=lookAty;
	this->lookPoint[2]=lookAtz;

	GLdouble pos[3] = {posx, posy, posz};

	GLdouble up[3] = {upx, upy, upz};
	GLdouble look[3] = {lookAtx, lookAty, lookAtz};
	GLdouble looklen = sqrt(pow(look[0], 2) + pow(look[1], 2) + pow(look[2], 2));
	//*eyePoint = *pos;

	for (int i = 0; i < 3; i++)
	{
		w[i] = ((-1 * look[i]) / looklen);
		if (w[i]==0) w[i]=0; //get rid of neg zeroes
	}

	GLdouble *vTemp = new GLdouble[3];

	for (int j = 0; j < 3; j++)
	{
		vTemp[j] = (up[j] - (dotProduct4d(up, w)*w[j]));	
	}

	GLdouble vLen = sqrt(pow(vTemp[0], 2) + pow(vTemp[1], 2) + pow(vTemp[2], 2));

	for (int k = 0; k < 3; k++)
	{
		v[k] = (vTemp[k] / vLen);
	}
	delete[] vTemp;

	GLdouble* uCross = crossProduct(v, w); //Since w and v are unit length and perpendicular, their cross product is also unit length.
	u[0] = uCross[0];
	u[1] = uCross[1];
	u[2] = uCross[2];

	delete[] uCross;// "And for each New array He declareth, He shall also have a matching Delete[], as a Shephard has a matching Flock" Stroustrup, 19:4

	//Pn = Vertex((posx + (near*w[0])), (posy + (near*w[1])), (posz + (near*w[2])));

	/*printf("INSIDE OF CONSTRUCTOR\n");
	printf("v is {%g, %g, %g}\n", v[0], v[1], v[2]);
	printf("w is {%g, %g, %g}\n", w[0], w[1], w[2]);
	printf("u is {%g, %g, %g}\n", u[0], u[1], u[2]);*/
}


//first translate camera to origin (or rather, its near plane)
const void Camera::transform() //accepts a pointer to a deque of Shapes. Multiplies each element of each shape's vertex data structure by the translation matrix T, thereby translating the camera to the world's origin...
{
	//printf("Moving camera to {%g, %g, %g}\n", pX, pY, pZ);
	GLdouble Ttrans[16] = {
		                   1.0,       0.0,       0.0,       0.0, 
	                       0.0,       1.0,       0.0,       0.0,
	                       0.0,       0.0,       1.0,       0.0,
	                       (-1*pX),   (-1*pY),   (-1*pZ),   1.0,
	                      };
    
	glMultMatrixd(Ttrans); //update matrix stack           
}


//then rotate the (u,v,w) vectors into (x, y, z)
const void Camera::align()
{
	/*printf("INSIDE OF ALIGN\n");
	printf("v is {%g, %g, %g}\n", v[0], v[1], v[2]);
	printf("w is {%g, %g, %g}\n", w[0], w[1], w[2]);
	printf("u is {%g, %g, %g}\n", u[0], u[1], u[2]);*/


	
	GLdouble Rrot[16] = {
		                 u[0], v[0], w[0], 0.0,
		                 u[1], v[1], w[1], 0.0,
		                 u[2], v[2], w[2], 0.0,
		                 0.0,  0.0,  0.0,  1.0,
		                }; 
    
	glMultMatrixd(Rrot); //update matrix stack
}

//Sortho
const void Camera::scale(GLdouble farCP, GLdouble height, GLdouble width, GLdouble nearCP)
{
	//modify via user-changable parameters:
	height+=dH;
	nearCP+=dOrthoNCP;
	farCP+=dOrthoFCP;

	if (width<0 || height<0 || farCP<0) return;

	GLdouble Sortho[16] = {
		                   (2/width), 0.0,        0.0,       0.0,
		                   0.0,       (2/height), 0.0,       0.0,
		                   0.0,       0.0,        (1/farCP), 0.0,
		                   0.0,       0.0,        0.0,       1.0,
		                  };

    //GLdouble *S_ptr = &S;
	glMultMatrixd(Sortho); //push the scaling matrix S onto the stack in order to shrink the view wolume to its desired specs
}

//Sperspective
const void Camera::scalePerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	//modify via user-changable parameters:
	fovy+=dH;
	zNear+=dPerspNCP;
	zFar+=dPerspFCP;
	aspect+=dAspect;

	if (zFar<0) return;

	GLdouble **Sperspective;
	GLdouble c = (-zNear / zFar);
	GLdouble THETAh = RADIANS*fovy;
	GLdouble THETAw = THETAh * aspect;

	if ((c+1)==0) return; 

	GLdouble Sxyz[16] = {
						 (1/(tan(THETAw/2)*zFar)),       0.0,       0.0,     0.0,
						 0.0,       (1/(tan(THETAh/2)*zFar)),       0.0,     0.0,
						 0.0,                            0.0,  (1/zFar),     0.0,
						 0.0,                            0.0,       0.0,     1.0,
						};

    GLdouble Mpp[16] = {
    				    1.0, 0.0, 0.0,         0.0,
    					0.0, 1.0, 0.0,         0.0,
    					0.0, 0.0, (-1/(c+1)), -1.0,
    					0.0, 0.0, (c/(c+1)),   0.0,
    				   };

    //Sperspective = matrixMult(Mpp, Sxyz);
    glMultMatrixd(Mpp);
    glMultMatrixd(Sxyz); //update matrix stack
}

const void Camera::print() //print class information
{

}


void Camera::translateCamera(GLdouble x, GLdouble y, GLdouble z)
{
	this->pX-=x;
	this->pY-=y;
	this->pZ-=z;
	//this->transform();
}

void Camera::translateCameraLocalized(GLdouble x, GLdouble y, GLdouble z)
{
	float t = 2.0;

	if (y!=0)
	{
		this->pX+=(y*(t*u[0]));
		this->pY+=(y*(t*u[1]));
		this->pZ+=(y*(t*u[2]));		
	}
	else if (x!=0)
	{
		this->pX+=(x*(t*v[0]));
		this->pY+=(x*(t*v[1]));
		this->pZ+=(x*(t*v[2]));		
	}
	else
	{
		this->pX+=(z*(t*w[0]));
		this->pY+=(z*(t*w[1]));
		this->pZ+=(z*(t*w[2]));		
	}

}

void Camera::rotateCameraLocalized(GLdouble THETA, GLdouble x, GLdouble y, GLdouble z)
{
	GLdouble **Rnew;
	THETA *= (M_PI/180);

	GLdouble Rrot[4][4] = {
	                       {u[0], u[1], u[2], 0.0},
	                       {v[0], v[1], v[2], 0.0},
	                       {w[0], w[1], w[2], 0.0},
	                       {0.0,  0.0,  0.0,  1.0},
	                      }; 

	GLdouble rotateX[4][4] = {
							  {1.0, 0.0,		0.0,	         0.0}, 
							  {0.0, cos(THETA), (-1*sin(THETA)), 0.0}, 
							  {0.0, sin(THETA), cos(THETA),      0.0}, 
							  {0.0, 0.0,	    0.0,	         1.0}, 
							 };

	GLdouble rotateY[4][4] = {
							  {cos(THETA),       0.0, sin(THETA), 0.0}, 
						      {0.0,              1.0, 0.0,        0.0}, 
						      {(-1*sin(THETA)),  0.0, cos(THETA), 0.0}, 
						      {0.0,              0.0, 0.0,        1.0}, 
					         };

	GLdouble rotateZ[4][4] = {
							  {cos(THETA), (-1*sin(THETA)), 0.0, 0.0}, 
						      {sin(THETA), cos(THETA),      0.0, 0.0}, 
						      {0.0,		   0.0,		        1.0, 0.0}, 
						      {0.0,		   0.0,		        0.0, 1.0}, 
					         };
    
	if (x==1 || x==-1)
	{
		Rnew = matrixMult(rotateX, Rrot);
	}
	else if (y==1 || y==-1)
	{
		Rnew = matrixMult(rotateY, Rrot);
	}
	else
	{
		Rnew = matrixMult(rotateZ, Rrot);
	}

	this->u[0] = Rnew[0][0];
	this->u[1] = Rnew[0][1];
	this->u[2] = Rnew[0][2];

	this->v[0] = Rnew[1][0];
	this->v[1] = Rnew[1][1];
	this->v[2] = Rnew[1][2];

	this->w[0] = Rnew[2][0];
	this->w[1] = Rnew[2][1];
	this->w[2] = Rnew[2][2];

	for (int i = 0; i < 4; i++)
	{
		delete[] Rnew[i];
	}

	delete[] Rnew;

}

void Camera::decrementHeight()
{
	//this->u[0]-=dH;
	//this->u[1]-=dH;
	//this->u[2]-=dH;
	dH--;
	printf("Decreasing Cam Height: %g\n", dH);
}

void Camera::incrementHeight()
{
	//this->u[0]+=dH;
	//this->u[1]+=dH;
	//this->u[2]+=dH;
	dH++;
	printf("Increasing Cam Height: %g\n", dH);
}

void Camera::increaseNCP()
{
	this->dOrthoNCP++;
	this->dPerspNCP++;
}

void Camera::decreaseNCP()
{
	this->dOrthoNCP--;
	this->dPerspNCP--;
}

void Camera::increaseFCP()
{
	this->dOrthoFCP++;
	this->dPerspFCP++;
}

void Camera::decreaseFCP()
{
	this->dOrthoFCP--;
	this->dPerspFCP--;
}

void Camera::zoomIn()
{
	this->zoomAmt++;
}

void Camera::zoomOut()
{
	this->zoomAmt--;
}

void Camera::decreaseAR()
{
	this->dAspect--;
}

void Camera::increaseAR()
{
	this->dAspect++;
}

Camera::~Camera() 
{
	//delete[] w;
	//delete[] v;
	//delete[] u;
	printf("\n**echo @ camera destructor: CLEAN EXIT**\n\n");
}