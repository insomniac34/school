/*@Programmer: Tyler Raborn
 *@Descrip: Header file for my collection of matrix algebra functions.
*/

#include <GL/glut.h>
#include "Vertex.h"

#ifndef MATRIX_H
#define MATRIX_H

GLdouble* ztranslateMatrix4d(GLdouble, Vertex);

GLdouble* translateMatrix4d(GLdouble, Vertex); //generates the 4x4 transformation matrix utilizing homogenous coordinates {x, y, z, w} on a 3D plane.

GLdouble* rotateMatrix4d(GLdouble, Vertex);//accepts a number THETA and a 1x4 column vector as parameters; returns the 4x4 rotation matrix for THETA degrees about the Y-axis.

GLdouble* zrotateMatrix4d(GLdouble, Vertex);//accepts a number THETA and a 1x4 column vector as parameters; returns the 4x4 rotation matrix for THETA degrees about the Z-axis.

GLdouble* scalarVectormult4d(GLdouble, GLdouble[4]); //accepts a scalar and a 1x4 column vector as parameters; returns the 1x4 column vector that results from scalar multiplication on the two parameters.

GLdouble** matrixMult4d(GLdouble[4][4], GLdouble[4][4], Vertex); //Accepts a pair of two-dimensional double arrays A & B representing 4x4 matrices; returns the 4x4 matrix resulting from A*B.

GLdouble** matrixAdd4d(GLdouble[4][4], GLdouble[4][4]); //Accepts a pair of two-dimensional double arrays A & B representing 4x4 matrices; returns the 4x4 matrix resulting from A+B.

double* matrixVectorMult4d(GLdouble[4][4], GLdouble[4][4], GLdouble[4]);

double* triMatrixVectorMult4d(double A[4][4], double B[4][4], double C[4][4], double V[4]);

GLdouble dotProduct4d(GLdouble[4], GLdouble[4]);

Vertex* crossProduct4d(GLfloat[4], GLfloat[4]);

GLdouble* crossProduct3V(Vertex v1, Vertex v2, Vertex V3);

GLdouble* normalize(Vertex vector);

GLdouble* translateVertex(Vertex, GLdouble, GLdouble, GLdouble); //multiplies vertices by translation matrix specified by the x y and z coordinate params

GLdouble* vectorMatrixMult(Vertex, GLdouble[4][4]);

GLdouble* rotateVertex(Vertex v, GLdouble deg, GLdouble x, GLdouble y, GLdouble z);

GLdouble* rotateVertexLocalized(Vertex, GLdouble, GLdouble, GLdouble, GLdouble);

GLdouble* scaleVertex(Vertex, GLdouble, GLdouble, GLdouble);

#endif
