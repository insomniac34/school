/********************************************************************************
* @Programmer: Tyler Raborn
* @Course: CS1566 
* @Assignment: Assignment 2 - 3D Tesselation
* @Description: Methods & Constructors for my Shape class
* @Dev Environment: Microsoft Visual Studio 2012, Windows 8 Pro 64-bit
* @GPU: nVidia GeForce GTX 670 2GB
*********************************************************************************/

#define RADIANS M_PI / 180.0
#define window_h 400.0
#define window_w 400.0
#define MAX_RS 50
#define MAX_VS 50
#define cube_size 1
#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <GL/glut.h>
#include <cstdio>
#include "Shape.h"
#include "matrix.h"
#include "Vertex.h"
#include "Face.h"

const static float r1 = 2.4;
const static float r2 = 0.4;
const static float r = 2.0;



Shape::Shape()
{


}

Shape::Shape(int shapeType, GLdouble ray, int rs, int vs, GLdouble maxHeight) //vs = vertical stacks, rs = radial stacks
{
	//REMEMBER: resize() will resize the vector every time it is called unless it is the same size as the requested size...
	this->rs = rs;
	this->vs = vs;
	type = shapeType;

	position = Vertex(0.0, 0.0, 0.0);

	curAxis = Vertex(0.0, 1.0, 0.0);
	curXAxis = Vertex(1.0, 0.0, 0.0);
	curZAxis = Vertex(0.0, 0.0, 1.0);

	

	//initialize data structures
	for (int i = 0; i < MAX_VS; i++)
	{
		vertexArray.push_back(std::vector<Vertex>());
		faceArray.push_back(std::vector<Face>());
	}

	//CALCULATE VERTICES BASED ON SHAPE TYPE:

	if (type==0) //cube
	{
		int i;  
		for(i = 0; i < 4; i++)
		{
			/*
			cubeVertices[i][0] = maxHeight*cos((M_PI/4));
			cubeVertices[i][1] = -maxHeight*sin(i*(M_PI/2)+(M_PI/4));
			cubeVertices[i][2] = maxHeight*cos(i*(M_PI/2)+(M_PI/4));
			cubeVertices[i][3] = 1;

			cubeVertices[i+4][0] = -maxHeight*cos((M_PI/4));
			cubeVertices[i+4][1] = -maxHeight*sin(i*(M_PI/2)+(M_PI/4));
			cubeVertices[i+4][2] = maxHeight*cos(i*(M_PI/2)+(M_PI/4));
			cubeVertices[i+4][3] = 1;
			*/

			Vertex v0 = Vertex(maxHeight*cos((M_PI/4)), -maxHeight*sin(i*(M_PI/2)+(M_PI/4)), maxHeight*cos(i*(M_PI/2)+(M_PI/4)));

			Vertex v1 = Vertex(-maxHeight*cos((M_PI/4)), -maxHeight*sin(i*(M_PI/2)+(M_PI/4)), maxHeight*cos(i*(M_PI/2)+(M_PI/4)));

			vertexArray[i].push_back(v0);
			vertexArray[i].push_back(v1);

			this->vs = 4;
			this->rs = 2;
		
		}
	}

	else if (type == 1) //cylinder
	{
		printf("entering makeCylinder...\n");
		radius = .5;
		
		int i = 0;
		int finalh;
		GLdouble h;
		GLdouble THETA;
		for (h = 0; h < maxHeight; h += (maxHeight / vs)) //outer loop handles height change
		{
			for (THETA = 0.0; THETA < (2*M_PI); THETA += ((2*M_PI) / rs)) //inner loop handles circles
			{		
				Vertex curPos = Vertex(radius*cos(THETA), h, (-radius * sin(THETA))); //current position on unit circle

				GLdouble* rotResult = rotateMatrix4d(2*M_PI, curPos);
				Vertex rotate = Vertex(rotResult);
				GLdouble* transResult = translateMatrix4d(h, rotate);
				Vertex transform = Vertex(transResult); //translate up w/ respect to the Y axis via translation matrix

				vertexArray[i].push_back(transform);

				//matrix calculations allocated on heap; MUST be deleted after STL data structure makes hard copy:
				delete[] rotResult;
				delete[] transResult;
			}
			i++;
			finalh = h;
		}
		if (i != MAX_VS)
		{
			while (vs >= i) //correction loop
			{
				finalh += (maxHeight / vs);
				Vertex curPos = Vertex(radius*cos(THETA), finalh, (-radius * sin(THETA))); //current position on unit circle

				GLdouble* rotateResult = rotateMatrix4d(2*M_PI, curPos);
				Vertex rotate = Vertex(rotateResult); //apply rotation matrix
				GLdouble* transformResult = translateMatrix4d(finalh, rotate); 
				Vertex transform = Vertex(transformResult); //translate up w/ respect to the Y axis via translation matrix

				vertexArray[i].push_back(transform);

				//matrix calculations allocated on heap; MUST be deleted after STL data structure makes hard copy:
				delete[] rotateResult;
				delete[] transformResult;

				i++;
			}
		}
		dim = i;
		//printf("vertexArray calculations for CYLINDER complete: size of vertexArray: %d, memory address of FRONT object in vertexArray: %g\n", vertexArray.size()-1, &vertexArray.front());
	}

	else if (type == 2) //sphere, centered at the origin...sph_ray = 3 vs = 10, rs = 20
	{
		//rs=rs-1;
		GLdouble PHI, THETA;
		THETA = 0.0;
		PHI = 0.0;
		printf("starting sphere loop...\n");
		for (int i = 0; i < vs; i++) //Vertical Stacks
		{
			for (int j = 0; j < rs; j++) //Radial Slices
			{
				GLdouble V[4] = {r, 0.0, 0.0, 1.0};
				GLdouble Rz[4][4] = {
									 {cos(THETA),    (-1 * sin(THETA)),   0.0, 0.0},
						             {sin(THETA),    cos(THETA),          0.0, 0.0},
						             {0.0,           0.0,                 1.0, 0.0},
						             {0.0,           0.0,                 0.0, 1.0},
					                };
				GLdouble Ry[4][4] = {
									 {cos(PHI),            0.0,     sin(PHI),  0.0}, 
									 {0.0,                 1.0,     0.0,       0.0},
									 {(-1 * sin(PHI)),     0.0,     cos(PHI),  0.0},
									 {0.0,                 0.0,     0.0,       1.0},
									};

				//GLdouble* result = matrixVectorMult4d(Rz, Ry, V);
				GLdouble* result0 = vectorMatrixMult(V, Ry);
				GLdouble* result1 = vectorMatrixMult(Vertex(result0), Rz);
				vertexArray[i].push_back(Vertex(result1));
				delete[] result0; //matrix calculations allocated on heap; MUST be deleted after STL data structure makes hard copy
				delete[] result1;
				PHI+=((2*M_PI) / (rs)); //2pi / rs for default crap sphere
			}
			THETA+=((M_PI) / (vs-1)); //M_PI / vs for default crap sphere
		}
		printf("exiting sphere loop\n");
	}

	else if (type == 3) //torus
	{
		printf("Entering makeTorus\n");
		GLdouble PHI, THETA;
		THETA = 0.0;//((2*M_PI) / vs);
		PHI = 0.0;//((2*M_PI) / rs);
		for (int i = 0; i < vs; i++)
		{
			for (int j = 0; j < rs; j++)
			{
				GLdouble V[4] = {r2, 0.0, 0.0, 1.0};
				GLdouble Rz[4][4] = {
									 {cos(THETA),    (-1 * sin(THETA)),   0.0, 0.0},
						             {sin(THETA),    cos(THETA),          0.0, 0.0},
						             {0.0,           0.0,                 1.0, 0.0},
						             {0.0,           0.0,                 0.0, 1.0},
					                };
				GLdouble Ry[4][4] = {
									 {cos(PHI),            0.0,     sin(PHI),  0.0}, 
									 {0.0,                 1.0,     0.0,       0.0},
									 {(-1 * sin(PHI)),     0.0,     cos(PHI),  0.0},
									 {0.0,                 0.0,     0.0,       1.0},
									};
				 GLdouble T[4][4] = {
									 {1.0, 0.0, 0.0,  r1},
									 {0.0, 1.0, 0.0, 0.0},
									 {0.0, 0.0, 1.0, 0.0},
									 {0.0, 0.0, 0.0, 1.0},
								    };

				 //GLdouble* result = triMatrixVectorMult4d(Ry, T, Rz, V);
				 GLdouble* result0 = vectorMatrixMult(V, Rz);
				 GLdouble* result1 = vectorMatrixMult(Vertex(result0), T);
				 GLdouble* result2 = vectorMatrixMult(Vertex(result1), Ry);

				 vertexArray[i].push_back(Vertex(result2));
				 delete[] result0; //matrix calculations allocated on heap; MUST be deleted after STL data structure makes hard copy
				 delete[] result1;
				 delete[] result2;
				PHI+=((2*M_PI) / rs);
			}
			THETA+=((2*M_PI) / vs);
		}
	} 

	else if (type==4) //cone
	{
		int finalh; //holds the value of h after loop completion for use in correction; converts to int to ensure consistent intervals in correction loop.
		int i = 0;
		GLdouble h;
		GLdouble THETA;
		for (h = 0; h < maxHeight; h += (maxHeight / vs)) //outer loop handles height change
		{
			for (THETA = 0; THETA < (2*M_PI); THETA += ((2*M_PI) / rs)) //inner loop handles circles
			{		
				Vertex curPos = Vertex(h*cos(THETA), h, (-h * sin(THETA))); //current position on unit circle

				GLdouble* rotateResult = rotateMatrix4d(2*M_PI, curPos);
				Vertex rotate = Vertex(rotateResult); //apply rotation matrix
				GLdouble* transformResult = translateMatrix4d(h, rotate);
				Vertex transform = Vertex(transformResult); //translate up w/ respect to the Y axis via translation matrix

				vertexArray[i].push_back(transform);

				//matrix calculations allocated on heap; MUST be deleted after STL data structure makes hard copy:
				delete[] rotateResult;
				delete[] transformResult;
			}
			i++;
			finalh = h;
		}
		if (i != MAX_VS)
		{
			while (vs >= i) //correction loop
			{
				//printf("In correction loop; i = %d\n", i);
				finalh += (maxHeight / vs);

				Vertex curPos = Vertex(finalh*cos(THETA), finalh, (-finalh * sin(THETA))); //current position on unit circle

				GLdouble* rotateRes = rotateMatrix4d(2*M_PI, curPos);
				Vertex rotate = Vertex(rotateRes); //apply rotation matrix
				GLdouble* transformRes = translateMatrix4d(finalh, rotate);
				Vertex transform = Vertex(transformRes); //translate up w/ respect to the Y axis via translation matrix

				vertexArray[i].push_back(transform);

				//matrix calculations allocated on heap; MUST be deleted after STL data structure makes hard copy:
				delete[] rotateRes;
				delete[] transformRes;

				i++;
			}
		}
		dim = i;
	}
}

void Shape::drawCube(GLdouble vertices[][4], int renderMode, bool normalFlag)
{
	for (int i = 0; i < 6; i++)
	{
		printf("Attempting to access element %d in vector of size %d\n", i, faceArray.size());
		faceArray[i][0].draw(renderMode);
	}
	//renderMode = GL_LINE_LOOP;
	/*glColor3d(1.0, 0.0, 0.0);

	glBegin(renderMode);
	glVertex4d(vertexArray[0][0].x, vertexArray[0][0].y, vertexArray[0][0].z, vertexArray[0][0].w);
	glVertex4d(vertexArray[0][1].x, vertexArray[0][1].y, vertexArray[0][1].z, vertexArray[0][1].w);
	glVertex4d(vertexArray[1][1].x, vertexArray[1][1].y, vertexArray[1][1].z, vertexArray[1][1].w);
	glVertex4d(vertexArray[1][0].x, vertexArray[1][0].y, vertexArray[1][0].z, vertexArray[1][0].w);
	glEnd();

	faceArray[0].push_back(Face(&vertexArray[0][0], &vertexArray[0][1], &vertexArray[1][1], &vertexArray[1][0]));

	glBegin(renderMode);
	glVertex4d(vertexArray[1][0].x, vertexArray[1][0].y, vertexArray[1][0].z, vertexArray[1][0].w);
	glVertex4d(vertexArray[1][1].x, vertexArray[1][1].y, vertexArray[1][1].z, vertexArray[1][1].w);
	glVertex4d(vertexArray[2][1].x, vertexArray[2][1].y, vertexArray[2][1].z, vertexArray[2][1].w);
	glVertex4d(vertexArray[2][0].x, vertexArray[2][0].y, vertexArray[2][0].z, vertexArray[2][0].w);
	glEnd();

	faceArray[0].push_back(Face(&vertexArray[1][0], &vertexArray[1][1], &vertexArray[2][1], &vertexArray[2][0]));

	glBegin(renderMode);
	glVertex4d(vertexArray[2][0].x, vertexArray[2][0].y, vertexArray[2][0].z, vertexArray[2][0].w);
	glVertex4d(vertexArray[2][1].x, vertexArray[2][1].y, vertexArray[2][1].z, vertexArray[2][1].w);
	glVertex4d(vertexArray[3][1].x, vertexArray[3][1].y, vertexArray[3][1].z, vertexArray[3][1].w);
	glVertex4d(vertexArray[3][0].x, vertexArray[3][0].y, vertexArray[3][0].z, vertexArray[3][0].w);
	glEnd();

	faceArray[0].push_back(Face(&vertexArray[2][0], &vertexArray[2][1], &vertexArray[3][1], &vertexArray[3][0]));

	glBegin(renderMode);
	glVertex4d(vertexArray[3][0].x, vertexArray[3][0].y, vertexArray[3][0].z, vertexArray[3][0].w);
	glVertex4d(vertexArray[3][1].x, vertexArray[3][1].y, vertexArray[3][1].z, vertexArray[3][1].w);
	glVertex4d(vertexArray[0][1].x, vertexArray[0][1].y, vertexArray[0][1].z, vertexArray[0][1].w);
	glVertex4d(vertexArray[0][0].x, vertexArray[0][0].y, vertexArray[0][0].z, vertexArray[0][0].w);
	glEnd();

	faceArray[0].push_back(Face(&vertexArray[3][0], &vertexArray[3][1], &vertexArray[0][1], &vertexArray[0][0]));

	//sides

	glBegin(renderMode);
	glVertex4d(vertexArray[3][0].x, vertexArray[3][0].y, vertexArray[3][0].z, vertexArray[3][0].w);
	glVertex4d(vertexArray[0][0].x, vertexArray[0][0].y, vertexArray[0][0].z, vertexArray[0][0].w);
	glVertex4d(vertexArray[1][0].x, vertexArray[1][0].y, vertexArray[1][0].z, vertexArray[1][0].w);
	glVertex4d(vertexArray[2][0].x, vertexArray[2][0].y, vertexArray[2][0].z, vertexArray[2][0].w);
	glEnd();

	faceArray[0].push_back(Face(&vertexArray[3][0], &vertexArray[0][0], &vertexArray[1][0], &vertexArray[2][0]));

	glBegin(renderMode);
	glVertex4d(vertexArray[3][1].x, vertexArray[3][1].y, vertexArray[3][1].z, vertexArray[3][1].w);
	glVertex4d(vertexArray[0][1].x, vertexArray[0][1].y, vertexArray[0][1].z, vertexArray[0][1].w);
	glVertex4d(vertexArray[1][1].x, vertexArray[1][1].y, vertexArray[1][1].z, vertexArray[1][1].w);
	glVertex4d(vertexArray[2][1].x, vertexArray[2][1].y, vertexArray[2][1].z, vertexArray[2][1].w);
	glEnd();

	faceArray[0].push_back(Face(&vertexArray[3][1], &vertexArray[0][1], &vertexArray[1][1], &vertexArray[2][1]));*/
			

	/*glBegin(renderMode);
	glVertex4dv(vertices[0]);
	glVertex4dv(vertices[1]);
	glVertex4dv(vertices[2]);
	glVertex4dv(vertices[3]);
	glEnd();

	glBegin(renderMode);
	glVertex4dv(vertices[1]);
	glVertex4dv(vertices[5]);
	glVertex4dv(vertices[6]);
	glVertex4dv(vertices[2]);
	glEnd();

	glBegin(renderMode);
	glVertex4dv(vertices[5]);
	glVertex4dv(vertices[4]);
	glVertex4dv(vertices[7]);
	glVertex4dv(vertices[6]);
	glEnd();

	glBegin(renderMode);
	glVertex4dv(vertices[4]);
	glVertex4dv(vertices[0]);
	glVertex4dv(vertices[3]);
	glVertex4dv(vertices[7]);	
	glEnd();*/

	/*for (int i = 0; i < vs-1; i++)
	{
		for (int j = 0; j < rs-1; j++)
		{
			faceArray[i][j].draw(renderMode);
		}
	}*/

}

void Shape::drawCylinder(int renderMode, bool normalFlag)
{
	for (int i = 0; i < vs-1; i++) 
	{
		for (int j = 0; j < rs-1; j++) 
		{
			if ((i!=(vs-1)) && (j!=(rs-1)))
			{
				faceArray[i][j].draw(renderMode);
			}
			if ((i != 0) && (i != (vs-1)) && (j != 0) && ( j != (rs-1)))
			{	
				if (normalFlag==true)
				{
					Vertex v0 = faceArray[i][j].getFP0();
					Vertex v1 = faceArray[i][j].getFP1();
					Vertex v2 = faceArray[i][j].getFP2();
					Vertex v3 = faceArray[i][j].getFP3();

					GLdouble* v0cross = crossProduct3V(v3, v0, v1);
					GLdouble* v0normal = normalize(Vertex(v0cross));

					GLdouble* v1cross = crossProduct3V(v0, v1, v2);
					GLdouble* v1normal = normalize(Vertex(v1cross));

					GLdouble* v2cross = crossProduct3V(v1, v2, v3);
					GLdouble* v2normal = normalize(Vertex(v2cross));

					GLdouble* v3cross = crossProduct3V(v2, v3, v0);
					GLdouble* v3normal = normalize(Vertex(v3cross));

					Vertex v0norm = Vertex(v0normal);
					Vertex v1norm = Vertex(v1normal);
					Vertex v2norm = Vertex(v2normal);
					Vertex v3norm = Vertex(v3normal);

					glColor3f(0.0, 1.0, 0.0);

					/*glBegin(GL_LINE_LOOP);
					glVertex3f(v0.x, v0.y, v0.z);
					glVertex3f((v0.x + (.06 * v0norm.x)), (v0.y + (.06 * v0norm.y)), (v0.z + (.06 * v0norm.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v1.x, v1.y, v1.z);
					glVertex3f((v1.x + (.06 * v1norm.x)), (v1.y + (.06 * v1norm.y)), (v1.z + (.06 * v1norm.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v2.x, v2.y, v2.z);
					glVertex3f((v2.x + (.06 * v2norm.x)), (v2.y + (.06 * v2norm.y)), (v2.z + (.06 * v2norm.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v3.x, v3.y, v3.z);
					glVertex3f((v3.x + (.06 * v3norm.x)), (v3.y + (.06 * v3norm.y)), (v3.z + (.06 * v3norm.z)));
					glEnd();*/

					Vertex curNormal = Vertex(((v0norm.x + v1norm.x + v2norm.x + v3norm.x)/4), ((v0norm.y + v1norm.y + v2norm.y + v3norm.y)/4), ((v0norm.z + v1norm.z + v2norm.z + v3norm.z)/4));

					glBegin(GL_LINE_LOOP);
					glVertex3f(v2.x, v2.y, v2.z);
					glVertex3f((v2.x + (.06 * curNormal.x)), (v2.y + (.06 * curNormal.y)), (v2.z + (.06 * curNormal.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v0.x, v0.y, v0.z);
					glVertex3f((v0.x + (.06 * curNormal.x)), (v0.y + (.06 * curNormal.y)), (v0.z + (.06 * curNormal.z)));
					glEnd();

					delete[] v0cross;
					delete[] v0normal;
				
					delete[] v1cross;
					delete[] v1normal;

					delete[] v2cross;
					delete[] v2normal;

					delete[] v3cross;
					delete[] v3normal;
				}
			}
			if (j!=0 && (i==0 || i==(vs-1))) //calculate & draw special case faces for stitching together radial seem
			{
				glColor3f(1.0, 0.0, 0.0);
				glBegin(renderMode);
				glVertex4f(vertexArray[0][j].x, vertexArray[0][j].y, vertexArray[0][j].z, vertexArray[0][j].w);
				glVertex4f(vertexArray[vs-1][j].x, vertexArray[vs-1][j].y, vertexArray[vs-1][j].z, vertexArray[vs-1][j].w);
				glVertex4f(vertexArray[vs-1][j-1].x, vertexArray[vs-1][j-1].y, vertexArray[vs-1][j-1].z, vertexArray[vs-1][j-1].w);
				glVertex4f(vertexArray[0][j-1].x, vertexArray[0][j-1].y, vertexArray[0][j-1].z, vertexArray[0][j-1].w);
				glEnd();
			}
		}
		if (i!=0) //calculate & draw special case faces for stitching together vertical seem
		{
			glColor3f(1.0, 0.0, 0.0);
			glBegin(renderMode);
			glVertex4f(vertexArray[i][0].x, vertexArray[i][0].y, vertexArray[i][0].z, vertexArray[i][0].w);
			glVertex4f(vertexArray[i][rs-1].x, vertexArray[i][rs-1].y, vertexArray[i][rs-1].z, vertexArray[i][rs-1].w);
			glVertex4f(vertexArray[i-1][rs-1].x, vertexArray[i-1][rs-1].y, vertexArray[i-1][rs-1].z, vertexArray[i-1][rs-1].w);
			glVertex4f(vertexArray[i-1][0].x, vertexArray[i-1][0].y, vertexArray[i-1][0].z, vertexArray[i-1][0].w);
			glEnd();
		}
	}
	//final stitch hack
	glBegin(renderMode);
	glVertex4f(vertexArray[0][0].x, vertexArray[0][0].y, vertexArray[0][0].z, vertexArray[0][0].w);
	glVertex4f(vertexArray[0][rs-1].x, vertexArray[0][rs-1].y, vertexArray[0][rs-1].z, vertexArray[0][rs-1].w);
	glVertex4f(vertexArray[vs-1][rs-1].x, vertexArray[vs-1][rs-1].y, vertexArray[vs-1][rs-1].z, vertexArray[vs-1][rs-1].w);
	glVertex4f(vertexArray[vs-1][0].x, vertexArray[vs-1][0].y, vertexArray[vs-1][0].z, vertexArray[vs-1][0].w);
	glEnd();
}

void Shape::drawSphere(int renderMode, bool normalFlag)
{
	printf("drawsphere\n");
	//draw Face() objects, draw stitching quads, draw vertex normals
	for (int i = 0; i < vs-1; i++) 
	{
		for (int j = 0; j < rs-1; j++) 
		{
			if ((i!=(vs-1)) && (j!=(rs-1)))
			{
				//printf("Drawing p3: %g, %g, %g, %g\n", faceArray[i][j].facepoint3->x, faceArray[i][j].facepoint3->y, faceArray[i][j].facepoint3->z,faceArray[i][j].facepoint3->w);
				faceArray[i][j].draw(renderMode);
			}
			if ((i != 0) && (i != vs-1) && (j != 0) && ( j != rs-1))
			{
				//The normal at a vertex should be computed as the normalised sum of all the unit length normals for each face the vertex shares. 
				if (normalFlag==true)
				{
					Vertex v0 = faceArray[i][j].getFP0();
					Vertex v1 = faceArray[i][j].getFP1();
					Vertex v2 = faceArray[i][j].getFP2();
					Vertex v3 = faceArray[i][j].getFP3();

					GLdouble* v0cross = crossProduct3V(v3, v0, v1);
					GLdouble* v0normal = normalize(Vertex(v0cross));

					GLdouble* v1cross = crossProduct3V(v0, v1, v2);
					GLdouble* v1normal = normalize(Vertex(v1cross));

					GLdouble* v2cross = crossProduct3V(v1, v2, v3);
					GLdouble* v2normal = normalize(Vertex(v2cross));

					GLdouble* v3cross = crossProduct3V(v2, v3, v0);
					GLdouble* v3normal = normalize(Vertex(v3cross));

					Vertex v0norm = Vertex(v0normal);
					Vertex v1norm = Vertex(v1normal);
					Vertex v2norm = Vertex(v2normal);
					Vertex v3norm = Vertex(v3normal);

					glColor3f(0.0, 1.0, 0.0);

					/*glBegin(GL_LINE_LOOP);
					glVertex3f(v0.x, v0.y, v0.z);
					glVertex3f((v0.x + (.06 * v0norm.x)), (v0.y + (.06 * v0norm.y)), (v0.z + (.06 * v0norm.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v1.x, v1.y, v1.z);
					glVertex3f((v1.x + (.06 * v1norm.x)), (v1.y + (.06 * v1norm.y)), (v1.z + (.06 * v1norm.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v2.x, v2.y, v2.z);
					glVertex3f((v2.x + (.06 * v2norm.x)), (v2.y + (.06 * v2norm.y)), (v2.z + (.06 * v2norm.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v3.x, v3.y, v3.z);
					glVertex3f((v3.x + (.06 * v3norm.x)), (v3.y + (.06 * v3norm.y)), (v3.z + (.06 * v3norm.z)));
					glEnd();*/

					Vertex curNormal = Vertex(((v0norm.x + v1norm.x + v2norm.x + v3norm.x)/4), ((v0norm.y + v1norm.y + v2norm.y + v3norm.y)/4), ((v0norm.z + v1norm.z + v2norm.z + v3norm.z)/4));

					glBegin(GL_LINE_LOOP);
					glVertex3f(v1.x, v1.y, v1.z);
					glVertex3f((v1.x + (.06 * curNormal.x)), (v1.y + (.06 * curNormal.y)), (v1.z + (.06 * curNormal.z)));
					glEnd();

					delete[] v0cross;
					delete[] v0normal;
				
					delete[] v1cross;
					delete[] v1normal;

					delete[] v2cross;
					delete[] v2normal;

					delete[] v3cross;
					delete[] v3normal;
				}
			}
			if (j!=0 && (i==0 || i==(vs-1))) //calculate & draw special case faces for stitching together radial seem
			{
				//YELLOW
				/*glColor3f(0.0, 1.0, 0.0);
				glBegin(renderMode);
				glVertex4f(vertexArray[vs-1][j-1].x, vertexArray[vs-1][j-1].y, vertexArray[vs-1][j-1].z, vertexArray[vs-1][j-1].w);
				glVertex4f(vertexArray[0][j-1].x, vertexArray[0][j-1].y, vertexArray[0][j-1].z, vertexArray[0][j-1].w);
				glVertex4f(vertexArray[vs-1][j].x, vertexArray[vs-1][j].y, vertexArray[vs-1][j].z, vertexArray[vs-1][j].w);
				glVertex4f(vertexArray[0][j].x, vertexArray[0][j].y, vertexArray[0][j].z, vertexArray[0][j].w);
				glEnd();*/
			}
		}
		if (i!=0) //calculate & draw special case faces for stitching together vertical seem
		{
			//GREEN
			glColor3f(1.0, 0.0, 0.0);
			glBegin(renderMode);
			glVertex4f(vertexArray[i][0].x, vertexArray[i][0].y, vertexArray[i][0].z, vertexArray[i][0].w);
			glVertex4f(vertexArray[i][rs-1].x, vertexArray[i][rs-1].y, vertexArray[i][rs-1].z, vertexArray[i][rs-1].w);
			glVertex4f(vertexArray[i-1][rs-1].x, vertexArray[i-1][rs-1].y, vertexArray[i-1][rs-1].z, vertexArray[i-1][rs-1].w);
			glVertex4f(vertexArray[i-1][0].x, vertexArray[i-1][0].y, vertexArray[i-1][0].z, vertexArray[i-1][0].w);
			glEnd();
		}

	}
	/*glColor3f(1.0, 0.0, 0.0);
	//final stitch hack
	glBegin(renderMode);
	glVertex4f(vertexArray[vs-1][0].x, vertexArray[vs-1][0].y, vertexArray[vs-1][0].z, vertexArray[vs-1][0].w);
	glVertex4f(vertexArray[0][0].x, vertexArray[0][0].y, vertexArray[0][0].z, vertexArray[0][0].w);
	glVertex4f(vertexArray[0][rs-1].x, vertexArray[0][rs-1].y, vertexArray[0][rs-1].z, vertexArray[0][rs-1].w);
	glVertex4f(vertexArray[vs-1][rs-1].x, vertexArray[vs-1][rs-1].y, vertexArray[vs-1][rs-1].z, vertexArray[vs-1][rs-1].w);
	glEnd();*/
}

void Shape::drawTorus(int renderMode, bool normalFlag)
{
	for (int i = 0; i < vs-1; i++) 
	{
		for (int j = 0; j < rs-1; j++) 
		{
			if ((i!=(vs-1)) && (j!=(rs-1)))
			{
				faceArray[i][j].draw(renderMode);
			}
			if ((i != 0) && (i != (vs-1)) && (j != 0) && ( j != (rs-1)))
			{	
				if (normalFlag==true)
				{
					Vertex v0 = faceArray[i][j].getFP0();
					Vertex v1 = faceArray[i][j].getFP1();
					Vertex v2 = faceArray[i][j].getFP2();
					Vertex v3 = faceArray[i][j].getFP3();

					GLdouble* v0cross = crossProduct3V(v3, v0, v1);
					GLdouble* v0normal = normalize(Vertex(v0cross));

					GLdouble* v1cross = crossProduct3V(v0, v1, v2);
					GLdouble* v1normal = normalize(Vertex(v1cross));

					GLdouble* v2cross = crossProduct3V(v1, v2, v3);
					GLdouble* v2normal = normalize(Vertex(v2cross));

					GLdouble* v3cross = crossProduct3V(v2, v3, v0);
					GLdouble* v3normal = normalize(Vertex(v3cross));

					Vertex v0norm = Vertex(v0normal);
					Vertex v1norm = Vertex(v1normal);
					Vertex v2norm = Vertex(v2normal);
					Vertex v3norm = Vertex(v3normal);

					glColor3f(0.0, 1.0, 0.0);

					/*glBegin(GL_LINE_LOOP);
					glVertex3f(v0.x, v0.y, v0.z);
					glVertex3f((v0.x + (.06 * v0norm.x)), (v0.y + (.06 * v0norm.y)), (v0.z + (.06 * v0norm.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v1.x, v1.y, v1.z);
					glVertex3f((v1.x + (.06 * v1norm.x)), (v1.y + (.06 * v1norm.y)), (v1.z + (.06 * v1norm.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v2.x, v2.y, v2.z);
					glVertex3f((v2.x + (.06 * v2norm.x)), (v2.y + (.06 * v2norm.y)), (v2.z + (.06 * v2norm.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v3.x, v3.y, v3.z);
					glVertex3f((v3.x + (.06 * v3norm.x)), (v3.y + (.06 * v3norm.y)), (v3.z + (.06 * v3norm.z)));
					glEnd();*/

					Vertex curNormal = Vertex(((v0norm.x + v1norm.x + v2norm.x + v3norm.x)/4), ((v0norm.y + v1norm.y + v2norm.y + v3norm.y)/4), ((v0norm.z + v1norm.z + v2norm.z + v3norm.z)/4));

					glBegin(GL_LINE_LOOP);
					glVertex3f(v2.x, v2.y, v2.z);
					glVertex3f((v2.x + (.06 * curNormal.x)), (v2.y + (.06 * curNormal.y)), (v2.z + (.06 * curNormal.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v0.x, v0.y, v0.z);
					glVertex3f((v0.x + (.06 * curNormal.x)), (v0.y + (.06 * curNormal.y)), (v0.z + (.06 * curNormal.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v1.x, v1.y, v1.z);
					glVertex3f((v1.x + (.06 * curNormal.x)), (v1.y + (.06 * curNormal.y)), (v1.z + (.06 * curNormal.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v1.x, v1.y, v1.z);
					glVertex3f((v1.x + (.06 * curNormal.x)), (v1.y + (.06 * curNormal.y)), (v1.z + (.06 * curNormal.z)));
					glEnd();

					delete[] v0cross;
					delete[] v0normal;
				
					delete[] v1cross;
					delete[] v1normal;

					delete[] v2cross;
					delete[] v2normal;

					delete[] v3cross;
					delete[] v3normal;
				}

			}
			if (j!=0 && (i==0 || i==(vs-1))) //calculate & draw special case faces for stitching together radial seem
			{
				glColor3f(1.0, 0.0, 0.0);
				glBegin(renderMode);
				glVertex4f(vertexArray[0][j].x, vertexArray[0][j].y, vertexArray[0][j].z, vertexArray[0][j].w);
				glVertex4f(vertexArray[vs-1][j].x, vertexArray[vs-1][j].y, vertexArray[vs-1][j].z, vertexArray[vs-1][j].w);
				glVertex4f(vertexArray[vs-1][j-1].x, vertexArray[vs-1][j-1].y, vertexArray[vs-1][j-1].z, vertexArray[vs-1][j-1].w);
				glVertex4f(vertexArray[0][j-1].x, vertexArray[0][j-1].y, vertexArray[0][j-1].z, vertexArray[0][j-1].w);
				glEnd();
			}
		}
		if (i!=0) //calculate & draw special case faces for stitching together vertical seem
		{
			glColor3f(1.0, 0.0, 0.0);
			glBegin(renderMode);
			glVertex4f(vertexArray[i][0].x, vertexArray[i][0].y, vertexArray[i][0].z, vertexArray[i][0].w);
			glVertex4f(vertexArray[i][rs-1].x, vertexArray[i][rs-1].y, vertexArray[i][rs-1].z, vertexArray[i][rs-1].w);
			glVertex4f(vertexArray[i-1][rs-1].x, vertexArray[i-1][rs-1].y, vertexArray[i-1][rs-1].z, vertexArray[i-1][rs-1].w);
			glVertex4f(vertexArray[i-1][0].x, vertexArray[i-1][0].y, vertexArray[i-1][0].z, vertexArray[i-1][0].w);
			glEnd();
		}
	}
	//final stitch hack:
	glBegin(renderMode);
	glVertex4f(vertexArray[0][0].x, vertexArray[0][0].y, vertexArray[0][0].z, vertexArray[0][0].w);
	glVertex4f(vertexArray[0][rs-1].x, vertexArray[0][rs-1].y, vertexArray[0][rs-1].z, vertexArray[0][rs-1].w);
	glVertex4f(vertexArray[vs-1][rs-1].x, vertexArray[vs-1][rs-1].y, vertexArray[vs-1][rs-1].z, vertexArray[vs-1][rs-1].w);
	glVertex4f(vertexArray[vs-1][0].x, vertexArray[vs-1][0].y, vertexArray[vs-1][0].z, vertexArray[vs-1][0].w);
	glEnd();

	

	glColor3f(0.0, 1.0, 0.0);

	

	

	/*glBegin(GL_LINE_LOOP);
	glVertex3f(curZAxis.x, curZAxis.y, curZAxis.z);
	glVertex3f((curZAxis.x + (.06 * curZAxis.x)), (curZAxis.y + (.06 * curZAxis.y)), (curZAxis.z + (.06 * curZAxis.z)));
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(curXAxis.x, curXAxis.y, curXAxis.z);
	glVertex3f((curXAxis.x + (.06 * curXAxis.x)), (curXAxis.y + (.06 * curXAxis.y)), (curXAxis.z + (.06 * curXAxis.z)));
	glEnd();*/
}

void Shape::drawCone(int renderMode, bool normalFlag)
{
	glColor3f(1.0, 0.0, 0.0);
	for (int i = 0; i < vs-1; i++) 
	{
		for (int j = 0; j < rs-1; j++) 
		{
			if ((i!=(vs-1)) && (j!=(rs-1)))
			{
				faceArray[i][j].draw(renderMode);
			}
			if ((i != 0) && (i != (vs-1)) && (j != 0) && ( j != (rs-1))) //must exclude boundary cases from vertex normal calculations
			{
				if (normalFlag == true) //if normals enabled, for each non-boundary [i][j]-vertex V in the array, 
				{
					Vertex v0 = faceArray[i][j].getFP0();
					Vertex v1 = faceArray[i][j].getFP1();
					Vertex v2 = faceArray[i][j].getFP2();
					Vertex v3 = faceArray[i][j].getFP3();

					GLdouble* v0cross = crossProduct3V(v3, v0, v1);
					GLdouble* v0normal = normalize(Vertex(v0cross));

					GLdouble* v1cross = crossProduct3V(v0, v1, v2);
					GLdouble* v1normal = normalize(Vertex(v1cross));

					GLdouble* v2cross = crossProduct3V(v1, v2, v3);
					GLdouble* v2normal = normalize(Vertex(v2cross));

					GLdouble* v3cross = crossProduct3V(v2, v3, v0);
					GLdouble* v3normal = normalize(Vertex(v3cross));

					Vertex v0norm = Vertex(v0normal);
					Vertex v1norm = Vertex(v1normal);
					Vertex v2norm = Vertex(v2normal);
					Vertex v3norm = Vertex(v3normal);

					glColor3f(0.0, 1.0, 0.0);

					glBegin(GL_LINE_LOOP);
					glVertex3f(v0.x, v0.y, v0.z);
					glVertex3f((v0.x + (.06 * v0norm.x)), (v0.y + (.06 * v0norm.y)), (v0.z + (.06 * v0norm.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v1.x, v1.y, v1.z);
					glVertex3f((v1.x + (.06 * v1norm.x)), (v1.y + (.06 * v1norm.y)), (v1.z + (.06 * v1norm.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v2.x, v2.y, v2.z);
					glVertex3f((v2.x + (.06 * v2norm.x)), (v2.y + (.06 * v2norm.y)), (v2.z + (.06 * v2norm.z)));
					glEnd();

					glBegin(GL_LINE_LOOP);
					glVertex3f(v3.x, v3.y, v3.z);
					glVertex3f((v3.x + (.06 * v3norm.x)), (v3.y + (.06 * v3norm.y)), (v3.z + (.06 * v3norm.z)));
					glEnd();

					delete[] v0cross;
					delete[] v0normal;
				
					delete[] v1cross;
					delete[] v1normal;

					delete[] v2cross;
					delete[] v2normal;

					delete[] v3cross;
					delete[] v3normal;
				}
			}
			if (j!=0 && (i==0 || i==(vs-1))) //calculate & draw special case faces for stitching together radial seem
			{
				glColor3f(1.0, 0.0, 0.0);
				glBegin(renderMode);
				glVertex4f(vertexArray[0][j].x, vertexArray[0][j].y, vertexArray[0][j].z, vertexArray[0][j].w);
				glVertex4f(vertexArray[vs-1][j].x, vertexArray[vs-1][j].y, vertexArray[vs-1][j].z, vertexArray[vs-1][j].w);
				glVertex4f(vertexArray[vs-1][j-1].x, vertexArray[vs-1][j-1].y, vertexArray[vs-1][j-1].z, vertexArray[vs-1][j-1].w);
				glVertex4f(vertexArray[0][j-1].x, vertexArray[0][j-1].y, vertexArray[0][j-1].z, vertexArray[0][j-1].w);
				glEnd();
			}
		}
		if (i!=0) //calculate & draw special case faces for stitching together vertical seem
		{
			glColor3f(1.0, 0.0, 0.0);
			glBegin(renderMode);
			glVertex4f(vertexArray[i][0].x, vertexArray[i][0].y, vertexArray[i][0].z, vertexArray[i][0].w);
			glVertex4f(vertexArray[i][rs-1].x, vertexArray[i][rs-1].y, vertexArray[i][rs-1].z, vertexArray[i][rs-1].w);
			glVertex4f(vertexArray[i-1][rs-1].x, vertexArray[i-1][rs-1].y, vertexArray[i-1][rs-1].z, vertexArray[i-1][rs-1].w);
			glVertex4f(vertexArray[i-1][0].x, vertexArray[i-1][0].y, vertexArray[i-1][0].z, vertexArray[i-1][0].w);
			glEnd();
		}

	}

	//final stitch hack
	glColor3f(1.0, 0.0, 0.0);
	glBegin(renderMode);
	glVertex4f(vertexArray[0][0].x, vertexArray[0][0].y, vertexArray[0][0].z, vertexArray[0][0].w);
	glVertex4f(vertexArray[0][rs-1].x, vertexArray[0][rs-1].y, vertexArray[0][rs-1].z, vertexArray[0][rs-1].w);
	glVertex4f(vertexArray[vs-1][rs-1].x, vertexArray[vs-1][rs-1].y, vertexArray[vs-1][rs-1].z, vertexArray[vs-1][rs-1].w);
	glVertex4f(vertexArray[vs-1][0].x, vertexArray[vs-1][0].y, vertexArray[vs-1][0].z, vertexArray[vs-1][0].w);
	glEnd();
}

void Shape::draw(int rs, int vs, int mode, bool normalFlag) //publicly visible method; directs control to relevant draw functions...
{
	normalFlag=true;
	if (type==0) drawCube(this->cubeVertices, mode, normalFlag);
	else if (type==1) drawCylinder(mode, normalFlag);
	else if (type==2) drawSphere(mode, normalFlag);
	else if (type==3) drawTorus(mode, normalFlag);
	else if (type==4) drawCone(mode, normalFlag);
}

void Shape::tesselate() //Method that handles tessellation of the objects
{
	if (this->type!=SHAPE_CUBE)
	{
		//calculate primary faces
		for (int i = 0; i < vs; i++) //Vertical Stacks
		{
			for (int j = 0; j < rs; j++) //Radial Slices
			{
				if ((i!=(vs-1)) && (j!=(rs-1)))
				{
					faceArray[i].push_back(Face(&vertexArray[i][j], &vertexArray[i][j+1], &vertexArray[i+1][j+1], &vertexArray[i+1][j]));
				}
			}
		}
	}
	else
	{
		faceArray[0].push_back(Face(&vertexArray[0][0], &vertexArray[0][1], &vertexArray[1][1], &vertexArray[1][0]));
		faceArray[1].push_back(Face(&vertexArray[1][0], &vertexArray[1][1], &vertexArray[2][1], &vertexArray[2][0]));
		faceArray[2].push_back(Face(&vertexArray[2][0], &vertexArray[2][1], &vertexArray[3][1], &vertexArray[3][0]));
		faceArray[3].push_back(Face(&vertexArray[3][0], &vertexArray[3][1], &vertexArray[0][1], &vertexArray[0][0]));
		faceArray[4].push_back(Face(&vertexArray[3][0], &vertexArray[0][0], &vertexArray[1][0], &vertexArray[2][0]));
		faceArray[5].push_back(Face(&vertexArray[3][1], &vertexArray[0][1], &vertexArray[1][1], &vertexArray[2][1]));
	}
}

void Shape::translate(GLfloat x, GLfloat y, GLfloat z) //translate shape by {x, y, z}
{
	for (int i = 0; i < vs; i++)
	{
		for (int j = 0; j < rs; j++)
		{
			GLdouble* temp = translateVertex(vertexArray[i][j], x, y, z);
			Vertex target = Vertex(temp);

			vertexArray[i][j].x = target.x;
			vertexArray[i][j].y = target.y;
			vertexArray[i][j].z = target.z;
			vertexArray[i][j].w = target.w;

			delete[] temp;	//result of matrix multiplication is on heap
		}
	}

	GLdouble* tempPos = translateVertex(position, x, y, z);
	position = Vertex(tempPos);
	delete[] tempPos;


	//update position
	

	//ensure matrix is also applied to shape main axis:
	/*GLdouble* axisRot = translateVertex(curAxis, x, y, z);
	curAxis = Vertex(axisRot);
	delete[] axisRot;
	

	GLdouble* XaxisRot = translateVertex(curXAxis, x, y, z);
	curXAxis = Vertex(XaxisRot);
	delete[] XaxisRot;
	

	GLdouble* YaxisRot = translateVertex(curZAxis, x, y, z);
	curZAxis = Vertex(YaxisRot);
	delete[] YaxisRot;
	

	GLdouble* tempPos = translateVertex(position, x, y, z);
	position = Vertex(tempPos);
	delete[] tempPos;*/

	//curAxis2 = Vertex(normalize(curAxis));


}

void Shape::rotate(GLfloat deg, GLfloat x, GLfloat y, GLfloat z) //x/y/z are flags denoting the rotational direction
{
	char axis = '0';
	if (x!=0) axis = 'X';
	else if (y!=0) axis = 'Y';
	else if (z!=0) axis = 'Z';

	//printf("Rotating shape by %g degrees about the %c axis\n", deg, axis);
	for (int i = 0; i < vs; i++)
	{
		for (int j = 0; j < rs; j++)
		{
			GLdouble* temp = rotateVertex(vertexArray[i][j], deg, x, y, z);
			Vertex target = Vertex(temp);

			vertexArray[i][j].x = target.x;
			vertexArray[i][j].y = target.y;
			vertexArray[i][j].z = target.z;
			vertexArray[i][j].w = target.w;

			delete[] temp;	//result of matrix multiplication is on heap

		}
	}
	
	//ensure matrix is also applied to shape main axis:
	GLdouble* axisRot = rotateVertex(curAxis, deg, x, y, z);
	curAxis = Vertex(axisRot);
	delete[] axisRot;


	GLdouble* XaxisRot = rotateVertex(curXAxis, deg, x, y, z);
	curXAxis = Vertex(XaxisRot);
	delete[] XaxisRot;

	GLdouble* YaxisRot = rotateVertex(curZAxis, deg, x, y, z);
	curZAxis = Vertex(YaxisRot);
	delete[] YaxisRot;

	//GLdouble* axisRot2 = rotateVertex(curAxis2, deg, x, y, z);
	//curAxis2 = Vertex(normalize(curAxis));
	//delete[] axisRot2;

	GLdouble* tempPos = rotateVertex(position, deg, x, y, z);
	position = Vertex(tempPos);
	delete[] tempPos;



}

void Shape::revolve(GLfloat deg, GLfloat x, GLfloat y, GLfloat z)
{
	//SAVE POSITION VARIABLES
	GLdouble xval = position.x;
	GLdouble yval = position.y;
	GLdouble zval = position.z;
	Vertex tempPos = Vertex(xval, yval, zval);

	//Translate object to the origin via position negated
	printf("Revolving...\n");
	translate((-1*position.x), (-1*position.y), (-1*position.z));

	for (int i = 0; i < vs; i++)
	{
		for (int j = 0; j < rs; j++)
		{
			GLdouble* temp = rotateVertexLocalized(vertexArray[i][j], deg, curAxis.x, curAxis.y, curAxis.z);
			Vertex target = Vertex(temp);

			vertexArray[i][j].x = target.x;
			vertexArray[i][j].y = target.y;
			vertexArray[i][j].z = target.z;
			vertexArray[i][j].w = target.w;

			delete[] temp;	//result of matrix multiplication is on heap
		}
	}

	translate(tempPos.x, tempPos.y, tempPos.z);



}

void Shape::scale(GLfloat x, GLfloat y, GLfloat z)
{
	printf("entering Scale\n");
	for (int i = 0; i < vs; i++)
	{
		for (int j = 0; j < rs; j++)
		{
			GLdouble* temp = scaleVertex(vertexArray[i][j], x, y, z);
			Vertex target = Vertex(temp);

			vertexArray[i][j].x = target.x;
			vertexArray[i][j].y = target.y;
			vertexArray[i][j].z = target.z;
			vertexArray[i][j].w = target.w;

			delete[] temp;	//result of matrix multiplication is on heap
		}
	}

	GLdouble* tempPos = scaleVertex(position, x, y, z);
	position = Vertex(tempPos);
	delete[] tempPos;

	//ensure matrix is also applied to shape main axis:
	/*GLdouble* axisRot = rotateVertex(curAxis, x, y, z);
	curAxis = Vertex(axisRot);
	delete[] axisRot;


	GLdouble* XaxisRot = rotateVertex(curXAxis, deg, x, y, z);
	curXAxis = Vertex(XaxisRot);
	delete[] XaxisRot;

	GLdouble* YaxisRot = rotateVertex(curZAxis, deg, x, y, z);
	curZAxis = Vertex(YaxisRot);
	delete[] YaxisRot;*/
}

void Shape::arbitraryRotate(GLdouble deg, GLdouble ax, GLdouble ay, GLdouble az, GLdouble cx, GLdouble cy, GLdouble cz)
{
	//normalize target axis:
	GLdouble* tempNorm = normalize(Vertex(ax, ay, az));
	Vertex normalizedAxis = Vertex(tempNorm);

	//translate shape to axis:
	translate((-1*cx), (-1*cy), (-1*cz));
	
	//apply rodrigues formula:
	for (int i = 0; i < vs; i++)
	{
		for (int j = 0; j < rs; j++)
		{
			GLdouble* tempRotate = rotateVertexLocalized(vertexArray[i][j], deg, normalizedAxis.x, normalizedAxis.y, normalizedAxis.z);
			Vertex target = Vertex(tempRotate);

			vertexArray[i][j].x = target.x;
			vertexArray[i][j].y = target.y;
			vertexArray[i][j].z = target.z;
			vertexArray[i][j].w = target.w;

			delete[] tempRotate; //deallocate ram
		}
	}

	//adjust axes accordingly:
	GLdouble* tempPos = rotateVertexLocalized(position, deg, normalizedAxis.x, normalizedAxis.y, normalizedAxis.z);
	position = Vertex(tempPos);
	delete[] tempPos;

	GLdouble* axisRot = rotateVertexLocalized(curAxis, deg, normalizedAxis.x, normalizedAxis.y, normalizedAxis.z);
	curAxis = Vertex(axisRot);
	delete[] axisRot;

	GLdouble* XaxisRot = rotateVertexLocalized(curXAxis, deg, normalizedAxis.x, normalizedAxis.y, normalizedAxis.z);
	curXAxis = Vertex(XaxisRot);
	delete[] XaxisRot;

	GLdouble* YaxisRot = rotateVertexLocalized(curZAxis, deg, normalizedAxis.x, normalizedAxis.y, normalizedAxis.z);
	curZAxis = Vertex(YaxisRot);
	delete[] YaxisRot;

	translate(cx, cy, cz);
}

int Shape::getType()
{
	return type;
}

Shape::~Shape() //destructor
{

}