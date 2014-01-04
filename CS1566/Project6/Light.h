//written by Tyler Raborn

#include <GL/glut.h>
#include "vec3.h"

#ifndef _LIGHT_H
#define _LIGHT_H

class Light
{
public:
	Light();
	Light(GLdouble[]);
	~Light();

	GLdouble *getPos();
	vec3 *getDir();
	GLdouble *getAmbient();
	GLdouble *getDiffuse();
	GLdouble *getEmission();
	GLdouble getShine();

	//light location:
	GLdouble position[3];
	vec3 dir;

	//light properties:
	GLdouble amb[3];
	GLdouble diff[3];
	GLdouble emission[3];
	GLdouble specular[3];
	GLdouble shine;

private:


};

#endif