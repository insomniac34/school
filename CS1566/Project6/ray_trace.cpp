//written by Tyler Raborn

//MSVS DEBUG UTILITIES:
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

/*
#define SHAPE_CUBE 1 //object id
#define SHAPE_HOUSE 2 //object id
#define SHAPE_SPHERE 3 //object id
#define SHAPE_CYLINDER 4 //object id
#define SHAPE_CONE 5 //object id
#define SHAPE_TORUS 6 //object id
*/

#include "ray_trace.h"

void initGL(void);
void displayScene(void);
void idle(void);
void resizeWindow(int width, int height);
void test();
void my_TimeOut(int);

int eyePointX, eyePointY, eyePointZ;
//GLdouble L_ambient[3];
//GLdouble L_diffuse[3];
//GLdouble L_specular[3];
//GLdouble L_position[4];
int lightCount = 0;

static const GLdouble identity4d[4][4] = {
									      {1.0, 0.0, 0.0, 0.0},
									      {0.0, 1.0, 0.0, 0.0},
									      {0.0, 0.0, 1.0, 0.0},
									      {0.0, 0.0, 0.0, 1.0},
								         };

static const GLdouble zeroMatrix[4][4] = {
							     	      {0.0, 0.0, 0.0, 0.0},
								          {0.0, 0.0, 0.0, 0.0},
								          {0.0, 0.0, 0.0, 0.0},
								          {0.0, 0.0, 0.0, 0.0},
								         };

Shape::Shape(GLdouble args[10], GLdouble lighting[13], int type) //standard constructor
{
	this->type = type;

	//initialize CTM with identity matrix
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i==j) 
			{
				CTMdir[i][j] = 1.0;
				CTMinverse[i][j] = 1.0;
				CTM[i][j] = 1.0;
			}
			else
			{
				CTM[i][j] = 0.0;
				CTMdir[i][j] = 0.0;
				CTMinverse[i][j] = 0.0;
			}
		}
	}

	//read in values from passed in array, transform shape accordingly. They are organized as follows: (rDeg, rX, rY, rZ), (tX, tY, tZ), (sX, sY, sZ)
	this->rotate(args[0], args[1], args[2], args[3]);
	this->translate(args[4], args[5], args[6]);
	this->scale(args[7], args[8], args[9]);

	//read in lighting values to be stored:
	this->shiny = lighting[0];

	this->emission[0] = lighting[1];
	this->emission[1] = lighting[2];
	this->emission[2] = lighting[3];
	this->emission[3] = lighting[4];

	this->amb[0] = lighting[5];
	this->amb[1] = lighting[6];
	this->amb[2] = lighting[7];
	this->amb[3] = lighting[8];

	this->diff[0] = lighting[9];
	this->diff[1] = lighting[10];
	this->diff[2] = lighting[11];
	this->diff[3] = lighting[12];
}

void Shape::translate(GLdouble x, GLdouble y, GLdouble z)
{
	x*=-1;
	y*=-1;
	z*=-1;
	GLdouble T[4][4] = {
						{1.0, 0.0, 0.0,   x},
						{0.0, 1.0, 0.0,   y},
						{0.0, 0.0, 1.0,   z},
						{0.0, 0.0, 0.0, 1.0},
					   };

	GLdouble Tinv[4][4] = {
						   {1.0, 0.0, 0.0,  -x},
						   {0.0, 1.0, 0.0,  -y},
						   {0.0, 0.0, 1.0,  -z},
						   {0.0, 0.0, 0.0, 1.0},
					      };

	GLdouble **tempCTM = matrixMult(CTM, T);
	GLdouble **result = matrixMult(CTMinverse, Tinv);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			CTM[i][j] = tempCTM[i][j];
			CTMinverse[i][j]=result[i][j];
		}
	}

	for (int i = 0; i < 4; i++)
	{
		delete[] result[i];
		delete[] tempCTM[i];
	}
	delete[] result;
	delete[] tempCTM;
}

void Shape::rotate(GLdouble THETA, GLdouble x, GLdouble y, GLdouble z)
{
	GLdouble **tempCTMinverse;
	GLdouble **tempCTMdir;
	GLdouble **tempCTM;

	///////////////////////////////Rotation Matrices////////////////////////////////////
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
    ////////////////////////////////////////////////////////////////////////////////////

	if (x!=0)
	{
		tempCTM = matrixMult(CTM, rotateX);

		THETA*=-1;
		GLdouble rotXinv[4][4] = {
								  {1.0, 0.0,		0.0,	         0.0}, 
								  {0.0, cos(THETA), (-1*sin(THETA)), 0.0}, 
								  {0.0, sin(THETA), cos(THETA),      0.0}, 
								  {0.0, 0.0,	    0.0,	         1.0}, 
								 };

		tempCTMdir = matrixMult(CTMdir, rotXinv);
		tempCTMinverse = matrixMult(CTMinverse, rotXinv);
	}
	else if (y!=0)
	{
		tempCTM = matrixMult(CTM, rotateY);

		THETA*=-1;
		GLdouble rotYinv[4][4] = {
								  {cos(THETA),       0.0, sin(THETA), 0.0}, 
								  {0.0,              1.0, 0.0,        0.0}, 
								  {(-1*sin(THETA)),  0.0, cos(THETA), 0.0}, 
								  {0.0,              0.0, 0.0,        1.0}, 
								 };

		tempCTMdir = matrixMult(CTMdir, rotYinv);
		tempCTMinverse = matrixMult(CTMinverse, rotYinv);
	}
	else if (z!=0)
	{
		tempCTM = matrixMult(CTM, rotateZ);

		THETA*=-1;
		GLdouble rotZinv[4][4] = {
								  {cos(THETA), (-1*sin(THETA)), 0.0, 0.0}, 
								  {sin(THETA), cos(THETA),      0.0, 0.0}, 
								  {0.0,		   0.0,		        1.0, 0.0}, 
								  {0.0,		   0.0,		        0.0, 1.0}, 
								 };

		tempCTMdir = matrixMult(CTMdir, rotZinv);
		tempCTMinverse = matrixMult(CTMinverse, rotZinv);
	}
	else
	{
		tempCTM = matrixMult(CTM, identity4d);
		tempCTMinverse = matrixMult(CTMinverse, identity4d);
		tempCTMdir = matrixMult(CTMdir, identity4d);
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			this->CTM[i][j] = tempCTM[i][j];
			this->CTMinverse[i][j] = tempCTMinverse[i][j];
			this->CTMdir[i][j] = tempCTMdir[i][j];
		}
	}

	for (int i = 0; i < 4; i++)
	{
		delete[] tempCTM[i];
		delete[] tempCTMinverse[i];
		delete[] tempCTMdir[i];
	}
	delete[] tempCTM;
	delete[] tempCTMinverse;
	delete[] tempCTMdir;
}

void Shape::scale(GLdouble sX, GLdouble sY, GLdouble sZ)
{
	GLdouble S[4][4] = {
		                {sX, 0.0, 0.0,  0.0}, 
					    {0.0, sY, 0.0,  0.0},
					    {0.0, 0.0, sZ,  0.0},
					    {0.0, 0.0, 0.0, 1.0},
	                   };

	GLdouble Sinv[4][4] = {
						   {(1/sX), 0.0,    0.0,     0.0}, 
						   {0.0,    (1/sY), 0.0,     0.0}, 
						   {0.0,    0.0,    (1/sZ),  0.0}, 
						   {0.0,    0.0,    0.0,     1.0},
	                      };

	GLdouble **tempCTM = matrixMult(CTM, S);
	GLdouble **tempCTMdir = matrixMult(CTMdir, Sinv);
	GLdouble **tempCTMinverse = matrixMult(CTMinverse, Sinv);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			this->CTM[i][j] = tempCTM[i][j];
			this->CTMinverse[i][j] = tempCTMinverse[i][j];
			this->CTMdir[i][j] = tempCTMdir[i][j];
		}
	}	

	for (int i = 0; i < 4; i++)
	{
		delete[] tempCTM[i];
		delete[] tempCTMinverse[i];
		delete[] tempCTMdir[i];
	}
	delete[] tempCTM;
	delete[] tempCTMinverse;
	delete[] tempCTMdir;
}

int Shape::getType()
{
	return this->type;
}

GLdouble** Shape::getCTM()
{
	return matrixMult(this->CTM, identity4d);
}

GLdouble** Shape::getCTMdir()
{
	return matrixMult(this->CTMdir, identity4d);
}

GLdouble** Shape::getCTMinverse()
{
	return matrixMult(this->CTMinverse, identity4d);
}

Shape::~Shape()
{

}

GLuint texId;
color_t screen[WINDOW_WIDTH * WINDOW_HEIGHT];
static int raster_x=0;
static int raster_y=0;

void test()
{
	/*glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	resizeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);*/
					
	int testCoordX = 200;
	int testCoordY = 200;
	while (testCoordX != 300 && testCoordY != 300)
	{
		plotPixel(screen, testCoordX, testCoordY, 1.0f, 0.0f, 0.0f, 1.0f);	
		testCoordX++;
		testCoordY++;
	}

	//glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, (void*)screen);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, texId);
	//glTexSubImage2D(GL_TEXTURE_2D, 0, raster_x, raster_y, 1, 1, GL_RGBA, GL_FLOAT, (void*)(raster_x*raster_y));
	//char ch;
	//std::cin >> ch;
}

void plotPixel(color_t *screen, int x, int y, float r, float g, float b, float a)
{
	screen[y*WINDOW_WIDTH + x].r = r;
	screen[y*WINDOW_WIDTH + x].g = g;
	screen[y*WINDOW_WIDTH + x].b = b;
	screen[y*WINDOW_WIDTH + x].a = a;
}

int main(int argc, char **argv)
{
	std::cout << "parsing file " << argv[1] << std::endl;
	std::string curLine;
	std::string flag = "#";
	std::string cam = "c";
	std::string lights = "l";
	
	std::ifstream myFile(argv[1]);
	if(myFile.is_open())
	{
		while (std::getline(myFile, curLine))
		{
			if (curLine.substr(0, 1)!=flag)
			{
				std::replace(curLine.begin(), curLine.end(), ',', ' '); //get rid of commas grumble grumble
				if (curLine.substr(0, 1)==lights)
				{
					lightCount++;
					std::cout << "READING LIGHTS: " << curLine << std::endl;
					int startFlag=0;
					int stage=0;
					int index;
					int type;

					std::string camPos;
					std::string camLook;
					std::string camUp;
					std::string lightPos;
					for (std::string::iterator iter = curLine.begin(); iter != curLine.end(); iter++)
					{
						if (((*iter)=='(') || ((*iter)==')'))
						{
							if (*iter==')')
							{
								if (stage==1) 
								{
									camPos = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);	
								}
								else if (stage==2) 
								{
									camLook = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);	
								}
								else if (stage==3) 
								{
									camUp = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);
								}
								else if (stage==4)
								{
									lightPos = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);
									break;
								}
								index = 0;
							}
							else 
							{
								index = std::distance(curLine.begin(), iter);
								stage++;
							}
						}
					}

					//proceed to read lighting values into global arrays
					std::stringstream pos_strm(camPos);
					int i = 0;
					GLdouble *L_ambient = new GLdouble[4];
					while(pos_strm>>L_ambient[i])
					{
						i++;
					}

					std::stringstream look_strm(camLook);
					i = 0;
					GLdouble *L_diffuse = new GLdouble[4];
					while (look_strm>>L_diffuse[i])
					{
						i++;
					}

					std::stringstream up_strm(camUp);
					i = 0;
					GLdouble *L_specular = new GLdouble[4];
					while (up_strm>>L_specular[i])
					{
						i++;
					}

					std::stringstream lightpos_strm(lightPos);
					i = 0;
					GLdouble *L_position = new GLdouble[4]; 
					while (lightpos_strm>>L_position[i])
					{
						i++;
					}

					GLdouble lightArgs[12] = {
												L_ambient[0], L_ambient[1], L_ambient[2],
												L_diffuse[0], L_diffuse[1], L_diffuse[2],
												L_specular[0], L_specular[1], L_specular[2], 
												L_position[0], L_position[1], L_position[2]
				                             };

					lightList.push_front(Light(lightArgs));

					std::cout << "position values read in: {" << L_position[0] << ", " << L_position[1] << ", " << L_position[2] << "}" <<  std::endl;
					std::cout << "ambient values read in: {" << L_ambient[0] << ", " << L_ambient[1] << ", " << L_ambient[2] << "}" << std::endl;
					std::cout << "diffuse values read in: {" << L_diffuse[0] << ", " << L_diffuse[1] << ", " << L_diffuse[2] << "}" << std::endl;
					std::cout << "specular values read in: {" << L_specular[0] << ", " << L_specular[1] << ", " << L_specular[2] << "}" << std::endl;

					delete[] L_ambient;
					delete[] L_diffuse;
					delete[] L_position;
					delete[] L_specular;
				}
				else if (curLine.substr(0, 1)==cam)
				{
					int startFlag=0;
					int stage=0;
					int index;
					int type;

					std::string camPos;
					std::string camLook;
					std::string camUp;
					for (std::string::iterator iter = curLine.begin(); iter != curLine.end(); iter++)
					{
						if (((*iter)=='(') || ((*iter)==')'))
						{
							if (*iter==')')
							{
								if (stage==1) 
								{
									camPos = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);	
								}
								else if (stage==2) 
								{
									camLook = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);	
								}
								else if (stage==3) 
								{
									camUp = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);
									break;
								}
								index = 0;
							}
							else 
							{
								index = std::distance(curLine.begin(), iter);
								stage++;
							}
						}
					}

					std::stringstream pos_strm(camPos);
					GLdouble *posVals = new GLdouble[4];
					int i = 0;
					while(pos_strm>>posVals[i])
					{
						i++;
					}

					std::stringstream look_strm(camLook);
					GLdouble *lookVals = new GLdouble[4];
					i = 0;
					while (look_strm>>lookVals[i])
					{
						i++;
					}

					std::stringstream up_strm(camUp);
					GLdouble *upVals = new GLdouble[4];
					i = 0;
					while (up_strm>>upVals[i])
					{
						i++;
					}

					//setup camera
                    glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();

					gluLookAt(
						      posVals[0], posVals[1], posVals[2],
						      lookVals[0], lookVals[1], lookVals[2],
							  upVals[0], upVals[1], upVals[2]
					         );	

				    printf("Camera vals: posVals: {%g, %g, %g}\n", posVals[0], posVals[1], posVals[2]);

					eyePointX = posVals[0];
					eyePointY = posVals[1];
					eyePointZ = posVals[2];

					delete[] posVals;
					delete[] lookVals;
					delete[] upVals;
				}
				else if (curLine.substr(0, 1)=="0" || curLine.substr(0, 1)=="1" || curLine.substr(0, 1)=="2" || curLine.substr(0, 1)=="3" || curLine.substr(0, 1)=="4" || curLine.substr(0, 1)=="5")
				{
					std::cout << "OBJECTS: " << curLine << std::endl;
					int stage = 0;
					int type;
					int index;

					//variables for creation of shape constructor array.
					GLdouble rDeg = 1.0;
					std::string trans;
					std::string rot;
					std::string scale;
					std::string mat_shiny;
					std::string mat_emission;
					std::string mat_amb;
					std::string mat_diffuse;
					//std::string mat_specular??

					int startFlag = 0;
					for (std::string::iterator iter = curLine.begin(); iter < curLine.end(); iter++)
					{
						if (startFlag==0)
						{
							char ch = (*iter); //get char pointed to by iterator
							printf("current ch: %c\n", ch);
							char *tempCh = new char[1]; //allocate new memory
							tempCh[0] = ch; //assign newly allocated memory to value of ch
							type = atoi(tempCh);//call atoi() to convert tempCh into a number
							delete[] tempCh; //free used memory.
							startFlag=1;
						}
						else if (((*iter)=='(') || ((*iter)==')'))
						{
							if (*iter==')')
							{
								if (stage==1) 
								{
									trans = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);	
								}
								else if (stage==2) 
								{
									scale = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);	
								}
								else if (stage==3) 
								{
									rot = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);
								}
								else if (stage==4)
								{
									mat_shiny = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);
								}
								else if (stage==5)
								{
									mat_emission = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);
								}
								else if (stage==6)
								{
									mat_amb = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);
								}
								else if (stage == 7)
								{
									mat_diffuse = curLine.substr(index+1, std::distance(curLine.begin(), iter)-index-1);
									break;
								}
								index = 0;
							}
							else 
							{
								index = std::distance(curLine.begin(), iter);
								stage++;
							}
						}
					}

					//std::cout << "type: " << type << " translate values: " << trans << " scaling values: " << scale << " rotation values " << rot << "\n" << std::endl;
					std::stringstream trans_strm(trans);
					GLdouble *transVals = new GLdouble[4];
					int i = 0;
					while(trans_strm>>transVals[i])
					{
						i++;
					}

					std::stringstream scale_strm(scale);
					GLdouble *scaleVals = new GLdouble[4];
					i = 0;
					while (scale_strm>>scaleVals[i])
					{
						i++;
					}

					std::stringstream rot_strm(rot);
					GLdouble *rotVals = new GLdouble[4];
					i = 0;
					while (rot_strm>>rotVals[i])
					{
						i++;
					}

					//mat_shiny:
					std::stringstream shiny_strm(mat_shiny);
					GLdouble shinyVal;
					shiny_strm>>shinyVal;

					//mat_emission:
					std::stringstream em_strm(mat_emission);
					GLdouble *emVals = new GLdouble[4];
					i = 0;
					while (em_strm>>emVals[i])
					{
						i++;
					}

					//mat_amb:
					std::stringstream amb_strm(mat_amb);
					GLdouble *ambVals = new GLdouble[4];
					i = 0;
					while (amb_strm>>ambVals[i])
					{
						i++;
					}

					//mat_diffuse:
					std::stringstream diff_strm(mat_diffuse);
					GLdouble *diffVals = new GLdouble[4];
					i = 0;
					while (diff_strm>>diffVals[i])
					{
						i++;
					}

					//variables for holding/organizing light data; to be passed into shape constructor
					GLdouble lightArgs[13] = {
						                      shinyVal, 
											  emVals[0], emVals[1], emVals[2], emVals[3], 
											  ambVals[0], ambVals[1], ambVals[2], ambVals[3], 
											  diffVals[0], diffVals[1], diffVals[2], diffVals[3]
					                         };
					
					//variables for shape data, organized as follows: (rDeg, rX, rY, rZ), (tX, tY, tZ), (sX, sY, sZ)
					GLdouble shapeArgs[10] = {
						                      rDeg, 
											  rotVals[0], rotVals[1], rotVals[2], 
						                      transVals[0], transVals[1], transVals[2], 
											  scaleVals[0], scaleVals[1], scaleVals[2]
					                         };

					//add new shape to global queue
					shapeList.push_front(Shape(shapeArgs, lightArgs, type));

					delete[] transVals;
					delete[] scaleVals;
					delete[] rotVals;
					delete[] emVals;
					delete[] ambVals;
					delete[] diffVals;
				}
			}
		}
		myFile.close();
	}
	else
	{
		std::cout << "Unable to open file\n" << std::endl;
		exit(0);
	}

	printf("\n///////////////////////////////////////////////////////////\n");
	printf("WORLD STATISTICS:\n");
	printf("Number of Lights read in: %d\n", lightList.size());
	printf("Number of Objects read in: %d\n", shapeList.size());
	for (int i = 0; i < shapeList.size(); i++)
	{
		std::string shtp;
		if (shapeList[i].getType()==SHAPE_SPHERE)
		{
			shtp = "SPHERE";
		}
		else
			shtp="unknown";
		printf("Shape entry %d is a %s\n", i, shtp.c_str());
	}
	printf("///////////////////////////////////////////////////////////\n");

	glutInit(&argc, argv);
	// depth buffer not really needed, but whatever
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutCreateWindow("Project 6");

	initGL();

	glutDisplayFunc(displayScene);
	glutReshapeFunc(resizeWindow);
	glutTimerFunc( 200, my_TimeOut, 0);
	glutIdleFunc(idle);

	glutMainLoop();

	

	return 0;
}

void initGL()
{
	int i, j;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);

	resizeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	// erase texture
	for (i=0;i<sizeof(screen)/sizeof(color_t);i++)
	{
		screen[i].r = 0.0f;
		screen[i].g = 0.0f;
		screen[i].b = 0.0f;
		screen[i].a = 1.0f;
	}

	// put a red square on the top left
	for (i=0; i < 32; i++)
	{
		for (j=0; j<32;j++)
		{
			plotPixel(screen, i, j, 1.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	// put a green square on the top right
	for (i=WINDOW_WIDTH-32-1; i<WINDOW_WIDTH; i++)
	{
		for (j=0; j<32;j++)
		{
			plotPixel(screen, i, j, 0.0f, 1.0f, 0.0f, 1.0f);
		}
	}

	// put a blue square on the bottom left
	for (i=0; i<32; i++)
	{
		for (j=WINDOW_HEIGHT-32-1; j<WINDOW_HEIGHT;j++)
		{
			plotPixel(screen, i, j, 0.0f, 0.0f, 1.0f, 1.0f);
		}
	}

	// put a white square on the bottom left
	for (i=WINDOW_WIDTH-32-1; i<WINDOW_WIDTH; i++)
	{
		for (j=WINDOW_HEIGHT-32-1; j<WINDOW_HEIGHT;j++)
		{
			plotPixel(screen, i, j, 1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	// create texture for drawing
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, (void*)screen);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void displayScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texId);

	// load orthographic projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();

	// restore projection
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glutSwapBuffers();
}

void idle()
{
	//test();
	//printf("inside of idle. Raster_x val: %g, Raster_y val: %g\n", raster_x, raster_y);
	
	float r, g, b, a;
	float x, y;
	
	//Vertex *nearPos = new Vertex(raster_x, raster_y, 0.0);
	//Vertex *farPos = new Vertex(raster_x, raster_y, 1.0);
	

	if (raster_x < WINDOW_WIDTH && raster_y < WINDOW_HEIGHT)
	{
		// first, do a ray trace at raster position (x,y). I made the coordinate system the same as in GLUT.
		GLint viewport[4];
		GLdouble modelMatrix[16];
		GLdouble projectionMatrix[16];

		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);

		GLdouble xNear, yNear, zNear;
		GLdouble xFar, yFar, zFar;
		xNear = eyePointX;
		yNear = eyePointY;
		zNear = eyePointZ;

		///////////////////////convert raster coords into 3d world coords/////////////////////////////////////
		gluUnProject(raster_x, raster_y, 1.0, modelMatrix, projectionMatrix, viewport, &xFar, &yFar, &zFar);
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		
		Vertex Peye = Vertex(xNear, yNear, zNear); //camera position
		Vertex farPos = Vertex(xFar, yFar, zFar); //gets point along far clipping plane
		vec3 dir = vec3(farPos, Peye);
		vec3 *dirNorm = dir.normalize();

		for (std::deque<Shape>::iterator it = shapeList.begin(); it != shapeList.end(); it++) //begin iterating over global deque of shapes, checking for intersections with each
		{
			//////////////////////Transform Ray Elements by CTM/////////////////////////////////
			Peye = Vertex(xNear, yNear, zNear);
			GLdouble **inverseCTM = it->getCTMinverse();
			GLdouble *tempPeye = vectorMatrixMult(Peye, inverseCTM);
			Peye.x=tempPeye[0];
			Peye.y=tempPeye[1];
			Peye.z=tempPeye[2];

			GLdouble **CTMdir_inv = it->getCTMdir();
			GLdouble *tempDir = vectorMatrixMult(*dirNorm, CTMdir_inv);
			dir.x = tempDir[0];
			dir.y = tempDir[1];
			dir.z = tempDir[2];
			
			delete[] tempPeye;
			delete[] tempDir;
			for (int i = 0; i < 4; i++)
			{
				delete[] inverseCTM[i];
				delete[] CTMdir_inv[i];
			}
			delete[] inverseCTM;
			delete[] CTMdir_inv;
			//////////////////////////////////////////////////////////////////////////////////

			if (it->getType()==SHAPE_CUBE)
			{
				GLdouble *result = new GLdouble[3];
				result[0] = 0;
				result[1] = 0;
				result[2] = 0;

				GLdouble zeroVal = 0;
				GLdouble cubeHits[3];
				
				//Cube = 6 individual 3d planes defined as follows:
				if (dir.x != 0) //LEFT/RIGHT
				{
					//LEFT//////////////////////////////////////////////
					zeroVal = (-0.5 - Peye.x) / dir.x;

					cubeHits[0] = dir.x*zeroVal;
					cubeHits[1] = dir.y*zeroVal;
					cubeHits[2] = dir.z*zeroVal;

					cubeHits[0] += Peye.x;
					cubeHits[1] += Peye.y;
					cubeHits[2] += Peye.z;

					if (cubeHits[1] <= 0.5 && cubeHits[2] >= -0.5 && cubeHits[2] <= 0.5 && cubeHits[2] >= -0.5) //if hit is in bounds
					{
						if (result[0] != 2)
						{
							result[0] += 1;
							int temp = result[0];
							result[temp] = zeroVal;
						}
					}
					////////////////////////////////////////////////////

					//RIGHT/////////////////////////////////////////////
					zeroVal = (0.5 - Peye.x) / dir.x;

					cubeHits[0] = dir.x*zeroVal;
					cubeHits[1] = dir.y*zeroVal;
					cubeHits[2] = dir.z*zeroVal;

					cubeHits[0] += Peye.x;
					cubeHits[1] += Peye.y;
					cubeHits[2] += Peye.z;			

					if (cubeHits[1] <= 0.5 && cubeHits[1] >= -0.5 && cubeHits[2] <= 0.5 && cubeHits[2] >= -0.5)
					{
						if (result[0] != 2)
						{
							result[0] += 1;
							int temp = result[0];
							result[temp] = zeroVal;
						}
					}
					////////////////////////////////////////////////////
				}
				if (dir.y != 0) //BOTTOM/TOP
				{
					//TOP///////////////////////////////////////////////
					zeroVal = (-0.5 - Peye.y) / dir.y;

					cubeHits[0] = dir.x*zeroVal;
					cubeHits[1] = dir.y*zeroVal;
					cubeHits[2] = dir.z*zeroVal;

					cubeHits[0] += Peye.x;
					cubeHits[1] += Peye.y;
					cubeHits[2] += Peye.z;

					if (cubeHits[0] <= 0.5 && cubeHits[0] >= -0.5 && cubeHits[2] <= 0.5 && cubeHits[2] >= -0.5)
					{
						if (result[0] != 2)
						{
							result[0] += 1;
							int temp = result[0];
							result[temp] = zeroVal;
						}
					}
					////////////////////////////////////////////////////

					//BOTTOM/////////////////////////////////////////////
					zeroVal = (0.5 - Peye.y) / dir.y;

					cubeHits[0] = dir.x*zeroVal;
					cubeHits[1] = dir.y*zeroVal;
					cubeHits[2] = dir.z*zeroVal;

					cubeHits[0] += Peye.x;
					cubeHits[1] += Peye.y;
					cubeHits[2] += Peye.z;

					if (cubeHits[0] <= 0.5 && cubeHits[0] >= -0.5 && cubeHits[2] <= 0.5 && cubeHits[2] >= -0.5)
					{
						if (result[0] != 2)
						{
							result[0] += 1;
							int temp = result[0];
							result[temp] = zeroVal;
						}
					}
					////////////////////////////////////////////////////			
				}
				if (dir.z != 0) //FRONT/BACK
				{
					//FRONT//////////////////////////////////////////////
					zeroVal = (-0.5 - Peye.z) / dir.z;

					cubeHits[0] = dir.x*zeroVal;
					cubeHits[1] = dir.y*zeroVal;
					cubeHits[2] = dir.z*zeroVal;

					cubeHits[0] += Peye.x;
					cubeHits[1] += Peye.y;
					cubeHits[2] += Peye.z;

					if (cubeHits[1] <= 0.5 && cubeHits[1] >= -0.5 && cubeHits[0] <= 0.5 && cubeHits[0] >= -0.5)
					{
						if (result[0] != 2)
						{
							result[0] += 1;
							int temp = result[0];
							result[temp] = zeroVal;
						}
					}
					////////////////////////////////////////////////////

					//BACK//////////////////////////////////////////////
					zeroVal = (0.5 - Peye.z) / dir.z;

					cubeHits[0] = dir.x*zeroVal;
					cubeHits[1] = dir.y*zeroVal;
					cubeHits[2] = dir.z*zeroVal;

					cubeHits[0] += Peye.x;
					cubeHits[1] += Peye.y;
					cubeHits[2] += Peye.z;

					if (cubeHits[1] <= 0.5 && cubeHits[1] >= -0.5 && cubeHits[0] <= 0.5 && cubeHits[0] >= -0.5)
					{
						if (result[0] != 2)
						{
							result[0] += 1;
							int temp = result[0];
							result[temp] = zeroVal;
						}
					}
					////////////////////////////////////////////////////	
				}

				//printf("CUBE: %d hits reported: {%g, %g, %g}\n", (GLint)result[0]);
				GLdouble hitX0, hitY0, hitZ0, hitX1, hitY1, hitZ1;

				hitX0 = Peye.x + (dir.x * result[1]);
				hitY0 = Peye.y + (dir.y * result[1]);
				hitZ0 = Peye.z + (dir.z * result[1]);

				hitX1 = Peye.x + (dir.x * result[2]);
				hitY1 = Peye.y + (dir.y * result[2]);
				hitZ1 = Peye.z + (dir.z * result[2]);

				Vertex hitA = Vertex(hitX0, hitY0, hitZ0);
				Vertex hitB = Vertex(hitX1, hitY1, hitZ1);

				int hitCount = 0;
				if (hitA.x <= 0.5 && hitA.x >= -0.5)
				{
					if (hitA.y <= 0.5 && hitA.y >= -0.5)
					{
						if (hitA.z <= 0.5 && hitA.z >= -0.5)
						{
							hitCount++;
							printf("Hit reported at {%g, %g, %g}\n", hitA.x, hitA.y, hitA.z);
						}
					}
				}
				if (hitB.x <= 0.5 && hitB.x >= -0.5)
				{
					if (hitB.y <= 0.5 && hitB.y >= -0.5)
					{
						if (hitB.z <= 0.5 && hitB.z >= -0.5)
						{
							hitCount++;
							printf("Hit reported at {%g, %g, %g}\n", hitB.x, hitB.y, hitB.z);
						}
					}
				}

				if (hitCount == 2 || hitCount == 1)
			    {
					//NOW calculate and apply lighting
					Vertex curShapePos = Vertex(0.0, 0.0, 0.0); //init @ origin
					GLdouble **shapeCurCTM = it->getCTM();
					GLdouble *tempShapePos = vectorMatrixMult(curShapePos, shapeCurCTM); //multiply above position by current shape's CTM to achieve its current position
					curShapePos.x = tempShapePos[0];
					curShapePos.y = tempShapePos[1];
					curShapePos.z = tempShapePos[2];

					for (int i = 0; i < 4; i++)
					{
						delete[] shapeCurCTM[i];
					}
					delete[] shapeCurCTM;
					delete[] tempShapePos;

					//////////////////calculate lighting values for current intersect point////////////////////////////////////////////////////////////
					GLdouble finalIntensity_R = 0.0; 
					GLdouble finalIntensity_G = 0.0;
					GLdouble finalIntensity_B = 0.0; //final intensity for wavelength LAMBDA
					GLdouble ambR = 0.0;
					GLdouble ambG = 0.0;
					GLdouble ambB = 0.0;
					GLdouble diffR = 0.0;
					GLdouble diffG = 0.0;
					GLdouble diffB = 0.0;

					vec3 N = vec3(hitA, curShapePos); 
					vec3 *Nnorm = N.normalize(); //the UNIT LENGTH surface normal at the point of intersection
					vec3 **L = new vec3*[lightList.size()]; //dynamically allocated array of vec3 objects, each of which will be subsequently allocated on the heap by calling normalize (see below)
					std::deque<Light>::iterator m; //the current light: to compute the final intensity we must add up contributions from ALL lights in the scene via the following for loop:					
					for (m = lightList.begin(); m != lightList.end(); m++) //sum values for all lights m <-> the set of all lights M, where M = {m0, m1, ..., mn}
					{
						//compound rgb values for each light parameter...for usage in the intensity equation...
						ambR+=m->amb[0];
						ambG+=m->amb[1];
						ambB+=m->amb[2];

						diffR+=m->diff[0];
						diffG+=m->diff[1];
						diffB+=m->diff[2];

						L[std::distance(lightList.begin(), m)] = vec3(m->position, hitA).normalize(); //each implicit entry within L contains a normalized vector allocated ON THE HEAP due to the call to normalize()
					}

					GLdouble globalAmbient[3] = {ambR, ambG, ambB}; //global intensity of ambient light...aka sum all ambient light RGB values...
					GLdouble objAmbient[4] = {it->amb[0], it->amb[1], it->amb[2], it->amb[3]}; //object's ambient color for wavelength LAMBDA; in our case the object's R, G or B value for ambient color...
					GLdouble mTotalIntensity[3] = {(ambR+diffR), (ambG+diffG), (ambB+diffB)}; //intensity of light m for wavelength LAMBDA; in our case the R, G, or B value of the light color for light m
					GLdouble globalDiffuse[3] = {diffR, diffG, diffB}; //global diffuse coefficient
					GLdouble objDiffuse[3] = {it->diff[0], it->diff[1], it->diff[2]}; //object's diffuse color wave length LAMBDA; in our case the OBJECT's R, G or B value for diffuse color.
						
					for (int i = 0; i < lightList.size(); i++) //for each ith light in the world, compound the respective RGB values...
					{	
						finalIntensity_R += ((globalAmbient[0]*it->amb[0]) + ((mTotalIntensity[0] * (globalDiffuse[0] * it->diff[0] * Nnorm->dot(*L[i])))));
						finalIntensity_G += ((globalAmbient[1]*it->amb[1]) + ((mTotalIntensity[1] * (globalDiffuse[1] * it->diff[1] * Nnorm->dot(*L[i])))));
						finalIntensity_B += ((globalAmbient[2]*it->amb[2]) + ((mTotalIntensity[2] * (globalDiffuse[2] * it->diff[2] * Nnorm->dot(*L[i])))));
					}

					GLdouble LAMBDA[3] = {finalIntensity_R, finalIntensity_G, finalIntensity_B};

					printf("CUBE: two hits detected! LAMBDA for Shape #%d = {%g, %g, %g}. Raster_x = %d, Raster_y = %d\n", (int)std::distance(shapeList.begin(), it), LAMBDA[0], LAMBDA[1], LAMBDA[2], raster_x, raster_y);
					plotPixel(screen, raster_x, raster_y, LAMBDA[0], LAMBDA[1], LAMBDA[2], 1.0f);
					glBindTexture(GL_TEXTURE_2D, texId);
					glTexSubImage2D(GL_TEXTURE_2D, 0, raster_x, raster_y, 1, 1, GL_RGBA, GL_FLOAT, (void*)(screen + (raster_y*WINDOW_WIDTH+raster_x)));
					///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					for (int i = 0; i < lightList.size(); i++)
					{
						delete L[i]; //delete each DYNAMICALLY ALLOCATED vec3 object resultant from the explicit call to each vec3's normalize() method contained within L[].
					}
					delete[] L;
					delete Nnorm;
				}
				delete[] result;
			}
			else if (it->getType()==SHAPE_SPHERE)
			{
				GLdouble *result;
				GLdouble a, b, c;
				a = (pow(dir.x, 2) + pow(dir.y, 2) + pow(dir.z, 2));
				b = (2*((Peye.x * dir.x) + (Peye.y * dir.y) + (Peye.z * dir.z)));
				c = (pow(Peye.x, 2) + pow(Peye.y, 2) + pow(Peye.z, 2))-1;

				result = getZeroes(a, b, c);

				if (result[2] == 1)
				{
					//compute 3d vertex of hit:
					GLdouble hitX0 = Peye.x + (dir.x * result[0]);
					GLdouble hitY0 = Peye.y + (dir.y * result[0]);
					GLdouble hitZ0 = Peye.z + (dir.z * result[0]);
					Vertex hitA = Vertex(hitX0, hitY0, hitZ0);

					Vertex curShapePos = Vertex(0.0, 0.0, 0.0); //init @ origin
					GLdouble **shapeCurCTM = it->getCTM();
					GLdouble *tempShapePos = vectorMatrixMult(curShapePos, shapeCurCTM); //multiply above position by current shape's CTM to achieve its current position
					curShapePos.x = tempShapePos[0];
					curShapePos.y = tempShapePos[1];
					curShapePos.z = tempShapePos[2];

					for (int i = 0; i < 4; i++)
					{
						delete[] shapeCurCTM[i];
					}
					delete[] shapeCurCTM;
					delete[] tempShapePos;

					//////////////////calculate lighting values for current intersect point////////////////////////////////////////////////////////////
					GLdouble finalIntensity_R = 0.0; 
					GLdouble finalIntensity_G = 0.0;
					GLdouble finalIntensity_B = 0.0; //final intensity for wavelength LAMBDA
					GLdouble ambR = 0.0;
					GLdouble ambG = 0.0;
					GLdouble ambB = 0.0;
					GLdouble diffR = 0.0;
					GLdouble diffG = 0.0;
					GLdouble diffB = 0.0;

					vec3 N = vec3(hitA, curShapePos); 
					vec3 *Nnorm = N.normalize(); //the UNIT LENGTH surface normal at the point of intersection
					vec3 **L = new vec3*[lightList.size()]; //dynamically allocated array of vec3 objects, each of which will be subsequently allocated on the heap by calling normalize (see below)
					std::deque<Light>::iterator m; //the current light: to compute the final intensity we must add up contributions from ALL lights in the scene via the following for loop:						
					for (m = lightList.begin(); m != lightList.end(); m++) //sum values for all lights m <-> the set of all lights M, where M = {m0, m1, ..., mn}
					{
						//compound rgb values for each light parameter...for usage in the intensity equation...
						ambR+=m->amb[0];
						ambG+=m->amb[1];
						ambB+=m->amb[2];

						diffR+=m->diff[0];
						diffG+=m->diff[1];
						diffB+=m->diff[2];

						L[std::distance(lightList.begin(), m)] = vec3(m->position, hitA).normalize(); //each implicit entry within L contains a normalized vector allocated ON THE HEAP due to the call to normalize()
					}

					GLdouble globalAmbient[3] = {ambR, ambG, ambB}; //global intensity of ambient light...aka sum all ambient light RGB values...
					GLdouble objAmbient[4] = {it->amb[0], it->amb[1], it->amb[2], it->amb[3]}; //object's ambient color for wavelength LAMBDA; in our case the object's R, G or B value for ambient color...
					GLdouble mTotalIntensity[3] = {(ambR+diffR), (ambG+diffG), (ambB+diffB)}; //intensity of light m for wavelength LAMBDA; in our case the R, G, or B value of the light color for light m
					GLdouble globalDiffuse[3] = {diffR, diffG, diffB}; //global diffuse coefficient
					GLdouble objDiffuse[3] = {it->diff[0], it->diff[1], it->diff[2]}; //object's diffuse color wave length LAMBDA; in our case the OBJECT's R, G or B value for diffuse color.
					
					for (int i = 0; i < lightList.size(); i++) //for each ith light in the world, compound the respective RGB values...
					{	
						finalIntensity_R += ((globalAmbient[0]*it->amb[0]) + ((mTotalIntensity[0] * (globalDiffuse[0] * it->diff[0] * Nnorm->dot(*L[i])))));
						finalIntensity_G += ((globalAmbient[1]*it->amb[1]) + ((mTotalIntensity[1] * (globalDiffuse[1] * it->diff[1] * Nnorm->dot(*L[i])))));
						finalIntensity_B += ((globalAmbient[2]*it->amb[2]) + ((mTotalIntensity[2] * (globalDiffuse[2] * it->diff[2] * Nnorm->dot(*L[i])))));
					}

					GLdouble LAMBDA[3] = {finalIntensity_R, finalIntensity_G, finalIntensity_B};

					printf("SPHERE: tangential hit detected! LAMBDA for Shape #%d = {%g, %g, %g}. Raster_x = %d, Raster_y = %d\n", (int)std::distance(shapeList.begin(), it), LAMBDA[0], LAMBDA[1], LAMBDA[2], raster_x, raster_y);
					plotPixel(screen, raster_x, raster_y, LAMBDA[0], LAMBDA[1], LAMBDA[2], 1.0f);
					glBindTexture(GL_TEXTURE_2D, texId);
					glTexSubImage2D(GL_TEXTURE_2D, 0, raster_x, raster_y, 1, 1, GL_RGBA, GL_FLOAT, (void*)(screen + (raster_y*WINDOW_WIDTH+raster_x)));
					///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					for (int i = 0; i < lightList.size(); i++)
					{
						delete L[i]; //delete each DYNAMICALLY ALLOCATED vec3 object resultant from the explicit call to each vec3's normalize() method contained within L[].
					}
					delete[] L;
					delete Nnorm;
				}
				else if (result[2]==2)
				{
					//compute 3d vertices of each hit:
					GLdouble hitX0 = Peye.x + (dir.x * result[0]);
					GLdouble hitY0 = Peye.y + (dir.y * result[0]);
					GLdouble hitZ0 = Peye.z + (dir.z * result[0]);
					Vertex hitA = Vertex(hitX0, hitY0, hitZ0);

					GLdouble hitX1 = Peye.x + (dir.x * result[1]);
					GLdouble hitY1 = Peye.y + (dir.y * result[1]);
					GLdouble hitZ1 = Peye.z + (dir.z * result[1]);
					Vertex hitB = Vertex(hitX1, hitY1, hitZ1);

					Vertex curShapePos = Vertex(0.0, 0.0, 0.0); //init @ origin
					GLdouble **shapeCurCTM = it->getCTM();
					GLdouble *tempShapePos = vectorMatrixMult(curShapePos, shapeCurCTM); //multiply above position by current shape's CTM to achieve its current position
					curShapePos.x = tempShapePos[0];
					curShapePos.y = tempShapePos[1];
					curShapePos.z = tempShapePos[2];

					for (int i = 0; i < 4; i++)
					{
						delete[] shapeCurCTM[i];
					}
					delete[] shapeCurCTM;
					delete[] tempShapePos;

					//////////////////calculate lighting values for current intersect point////////////////////////////////////////////////////////////
					GLdouble finalIntensity_R = 0.0; 
					GLdouble finalIntensity_G = 0.0;
					GLdouble finalIntensity_B = 0.0; //final intensity for wavelength LAMBDA
					GLdouble ambR = 0.0;
					GLdouble ambG = 0.0;
					GLdouble ambB = 0.0;
					GLdouble diffR = 0.0;
					GLdouble diffG = 0.0;
					GLdouble diffB = 0.0;

					vec3 N = vec3(hitA, curShapePos); 
					vec3 *Nnorm = N.normalize(); //the UNIT LENGTH surface normal at the point of intersection
					vec3 **L = new vec3*[lightList.size()]; //dynamically allocated array of vec3 objects, each of which will be subsequently allocated on the heap by calling normalize (see below)
					std::deque<Light>::iterator m; //the current light: to compute the final intensity we must add up contributions from ALL lights in the scene via the following for loop:					
					for (m = lightList.begin(); m != lightList.end(); m++) //sum values for all lights m <-> the set of all lights M, where M = {m0, m1, ..., mn}
					{
						//compound rgb values for each light parameter...for usage in the intensity equation...
						ambR+=m->amb[0];
						ambG+=m->amb[1];
						ambB+=m->amb[2];

						diffR+=m->diff[0];
						diffG+=m->diff[1];
						diffB+=m->diff[2];

						L[std::distance(lightList.begin(), m)] = vec3(m->position, hitA).normalize(); //each implicit entry within L contains a normalized vector allocated ON THE HEAP due to the call to normalize()
					}

					GLdouble globalAmbient[3] = {ambR+it->amb[0], ambG+it->amb[1], ambB+it->amb[2]}; //global intensity of ambient light...aka sum all ambient light RGB values...
					GLdouble objAmbient[4] = {it->amb[0], it->amb[1], it->amb[2], it->amb[3]}; //object's ambient color for wavelength LAMBDA; in our case the object's R, G or B value for ambient color...
					GLdouble mTotalIntensity[3] = {(ambR+diffR), (ambG+diffG), (ambB+diffB)}; //intensity of light m for wavelength LAMBDA; in our case the R, G, or B value of the light color for light m
					GLdouble globalDiffuse[3] = {diffR+it->diff[0], diffG+it->diff[1], diffB+it->diff[2]}; //global diffuse coefficient
					GLdouble objDiffuse[3] = {it->diff[0], it->diff[1], it->diff[2]}; //object's diffuse color wave length LAMBDA; in our case the OBJECT's R, G or B value for diffuse color.
					
					for (int i = 0; i < lightList.size(); i++) //for each ith light in the world, compound the respective RGB values...
					{	
						finalIntensity_R += ((globalAmbient[0]*it->amb[0]) + ((mTotalIntensity[0] * (globalDiffuse[0] * it->diff[0] * Nnorm->dot(*L[i])))));
						finalIntensity_G += ((globalAmbient[1]*it->amb[1]) + ((mTotalIntensity[1] * (globalDiffuse[1] * it->diff[1] * Nnorm->dot(*L[i])))));
						finalIntensity_B += ((globalAmbient[2]*it->amb[2]) + ((mTotalIntensity[2] * (globalDiffuse[2] * it->diff[2] * Nnorm->dot(*L[i])))));
					}

					GLdouble LAMBDA[3] = {finalIntensity_R, finalIntensity_G, finalIntensity_B};

					printf("SPHERE: two hits detected! LAMBDA for Shape #%d = {%g, %g, %g}. Raster_x = %d, Raster_y = %d\n", (int)std::distance(shapeList.begin(), it), LAMBDA[0], LAMBDA[1], LAMBDA[2], raster_x, raster_y);
					plotPixel(screen, raster_x, raster_y, LAMBDA[0], LAMBDA[1], LAMBDA[2], 1.0f);
					glBindTexture(GL_TEXTURE_2D, texId);
					glTexSubImage2D(GL_TEXTURE_2D, 0, raster_x, raster_y, 1, 1, GL_RGBA, GL_FLOAT, (void*)(screen + (raster_y*WINDOW_WIDTH+raster_x)));
					///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					for (int i = 0; i < lightList.size(); i++)
					{
						delete L[i]; //delete each DYNAMICALLY ALLOCATED vec3 object resultant from the explicit call to each vec3's normalize() method contained within L[].
					}
					delete[] L;
					delete Nnorm;
				}
				else
				{
					printf("Raster_x = %d, Raster_y = %d\n", raster_x, raster_y);
					//...stuff
				}
				delete[] result;
			}
			else if (it->getType()==SHAPE_CYLINDER)
			{

				GLdouble THETA = -8.0;
				GLdouble rotateX[4][4] = {
										  {1.0, 0.0,		0.0,	         0.0}, 
										  {0.0, cos(THETA), (-1*sin(THETA)), 0.0}, 
										  {0.0, sin(THETA), cos(THETA),      0.0}, 
										  {0.0, 0.0,	    0.0,	         1.0}, 
										 };		

				//multiply the elements of the ray by the above rotation matrix

				GLdouble *dir_rot = vectorMatrixMult(dir, rotateX);
				GLdouble *Peye_rot = vectorMatrixMult(Peye, rotateX);
		
				dir.x = dir_rot[0];
				dir.y = dir_rot[1];
				dir.z = dir_rot[2];

				Peye.x = Peye_rot[0];
				Peye.y = Peye_rot[1];
				Peye.z = Peye_rot[2];

				delete[] dir_rot;
				delete[] Peye_rot;

				//implement implicit equation for a cylinder
				GLdouble *result;
				GLdouble a, b, c;
				a = (pow(dir.x, 2) + pow(dir.y, 2));
				b = (2 * ((Peye.x * dir.x) + (Peye.y * dir.y)));
				c = ((pow(Peye.x, 2) + pow(Peye.y, 2))-1);
				result = getZeroes(a, b, c); //quadratic formula
				
				if (result[2] == 1)
				{
					//std::cout << "CYLINDER: TANGENTIAL HIT DETECTED!" << std::endl;

					//implement equation for a ray: r(t) = p + td
					GLdouble hitX0 = Peye.x + (dir.x * result[0]);
					GLdouble hitY0 = Peye.y + (dir.y * result[0]);
					GLdouble hitZ0 = Peye.z + (dir.z * result[0]);
					Vertex hitA = Vertex(hitX0, hitY0, hitZ0);


					if (hitA.z <= 1.0 && hitA.z >= -1.0)
					{
						Vertex curShapePos = Vertex(0.0, 0.0, 0.0); //init @ origin
						GLdouble **shapeCurCTM = it->getCTM();
						GLdouble *tempShapePos = vectorMatrixMult(curShapePos, shapeCurCTM); //multiply above position by current shape's CTM to achieve its current position
						curShapePos.x = tempShapePos[0];
						curShapePos.y = tempShapePos[1];
						curShapePos.z = tempShapePos[2];

						for (int i = 0; i < 4; i++)
						{
							delete[] shapeCurCTM[i];
						}
						delete[] shapeCurCTM;
						delete[] tempShapePos;

						//////////////////calculate lighting values for current intersect point////////////////////////////////////////////////////////////
						GLdouble finalIntensity_R = 0.0; 
						GLdouble finalIntensity_G = 0.0;
						GLdouble finalIntensity_B = 0.0; //final intensity for wavelength LAMBDA
						GLdouble ambR = 0.0;
						GLdouble ambG = 0.0;
						GLdouble ambB = 0.0;
						GLdouble diffR = 0.0;
						GLdouble diffG = 0.0;
						GLdouble diffB = 0.0;

						vec3 N = vec3(hitA, curShapePos); 
						vec3 *Nnorm = N.normalize(); //the UNIT LENGTH surface normal at the point of intersection
						vec3 **L = new vec3*[lightList.size()]; //dynamically allocated array of vec3 objects, each of which will be subsequently allocated on the heap by calling normalize (see below)
						std::deque<Light>::iterator m; //the current light: to compute the final intensity we must add up contributions from ALL lights in the scene via the following for loop:					
						for (m = lightList.begin(); m != lightList.end(); m++) //sum values for all lights m <-> the set of all lights M, where M = {m0, m1, ..., mn}
						{
							//compound rgb values for each light parameter...for usage in the intensity equation...
							ambR+=m->amb[0];
							ambG+=m->amb[1];
							ambB+=m->amb[2];

							diffR+=m->diff[0];
							diffG+=m->diff[1];
							diffB+=m->diff[2];

							L[std::distance(lightList.begin(), m)] = vec3(m->position, hitA).normalize(); //each implicit entry within L contains a normalized vector allocated ON THE HEAP due to the call to normalize()
						}

						GLdouble globalAmbient[3] = {ambR, ambG, ambB}; //global intensity of ambient light...aka sum all ambient light RGB values...
						GLdouble objAmbient[4] = {it->amb[0], it->amb[1], it->amb[2], it->amb[3]}; //object's ambient color for wavelength LAMBDA; in our case the object's R, G or B value for ambient color...
						GLdouble mTotalIntensity[3] = {(ambR+diffR), (ambG+diffG), (ambB+diffB)}; //intensity of light m for wavelength LAMBDA; in our case the R, G, or B value of the light color for light m
						GLdouble globalDiffuse[3] = {diffR, diffG, diffB}; //global diffuse coefficient
						GLdouble objDiffuse[3] = {it->diff[0], it->diff[1], it->diff[2]}; //object's diffuse color wave length LAMBDA; in our case the OBJECT's R, G or B value for diffuse color.
					
						for (int i = 0; i < lightList.size(); i++) //for each ith light in the world, compound the respective RGB values...
						{	
							finalIntensity_R += ((globalAmbient[0]*it->amb[0]) + ((mTotalIntensity[0] * (globalDiffuse[0] * it->diff[0] * Nnorm->dot(*L[i])))));
							finalIntensity_G += ((globalAmbient[1]*it->amb[1]) + ((mTotalIntensity[1] * (globalDiffuse[1] * it->diff[1] * Nnorm->dot(*L[i])))));
							finalIntensity_B += ((globalAmbient[2]*it->amb[2]) + ((mTotalIntensity[2] * (globalDiffuse[2] * it->diff[2] * Nnorm->dot(*L[i])))));
						}

						GLdouble LAMBDA[3] = {finalIntensity_R, finalIntensity_G, finalIntensity_B};

						printf("CYLINDER: tangential hit detected! LAMBDA for Shape #%d = {%g, %g, %g}. Raster_x = %d, Raster_y = %d\n", (int)std::distance(shapeList.begin(), it), LAMBDA[0], LAMBDA[1], LAMBDA[2], raster_x, raster_y);
						plotPixel(screen, raster_x, raster_y, LAMBDA[0], LAMBDA[1], LAMBDA[2], 1.0f);
						glBindTexture(GL_TEXTURE_2D, texId);
						glTexSubImage2D(GL_TEXTURE_2D, 0, raster_x, raster_y, 1, 1, GL_RGBA, GL_FLOAT, (void*)(screen + (raster_y*WINDOW_WIDTH+raster_x)));
						///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

						for (int i = 0; i < lightList.size(); i++)
						{
							delete L[i]; //delete each DYNAMICALLY ALLOCATED vec3 object resultant from the explicit call to each vec3's normalize() method contained within L[].
						}
						delete[] L;
						delete Nnorm;
					}
				}
				else if (result[2]==2)
				{
					//std::cout << "CYLINDER: TWO HITS DETECTED" << std::endl;
					//compute 3d vertices of each hit:

					GLdouble hitX0 = Peye.x + (dir.x * result[0]);
					GLdouble hitY0 = Peye.y + (dir.y * result[0]);
					GLdouble hitZ0 = Peye.z + (dir.z * result[0]);
					Vertex hitA = Vertex(hitX0, hitY0, hitZ0);

					//printf("Hit A = {%g, %g, %g}\n", hitA.x, hitA.y, hitA.z);

					GLdouble hitX1 = Peye.x + (dir.x * result[1]);
					GLdouble hitY1 = Peye.y + (dir.y * result[1]);
					GLdouble hitZ1 = Peye.z + (dir.z * result[1]);
					Vertex hitB = Vertex(hitX1, hitY1, hitZ1);

					if ((hitA.z <= 1.0 && hitA.z >= -1.0) && (hitB.z <= 1.0 && hitB.z >= -1.0))
					{

						Vertex curShapePos = Vertex(0.0, 0.0, 0.0); //init @ origin
						GLdouble **shapeCurCTM = it->getCTM();
						GLdouble *tempShapePos = vectorMatrixMult(curShapePos, shapeCurCTM); //multiply above position by current shape's CTM to achieve its current position
						curShapePos.x = tempShapePos[0];
						curShapePos.y = tempShapePos[1];
						curShapePos.z = tempShapePos[2];

						for (int i = 0; i < 4; i++)
						{
							delete[] shapeCurCTM[i];
						}
						delete[] shapeCurCTM;
						delete[] tempShapePos;

						//////////////////calculate lighting values for current intersect point////////////////////////////////////////////////////////////
						GLdouble finalIntensity_R = 0.0; 
						GLdouble finalIntensity_G = 0.0;
						GLdouble finalIntensity_B = 0.0; //final intensity for wavelength LAMBDA
						GLdouble ambR = 0.0;
						GLdouble ambG = 0.0;
						GLdouble ambB = 0.0;
						GLdouble diffR = 0.0;
						GLdouble diffG = 0.0;
						GLdouble diffB = 0.0;

						vec3 N = vec3(hitA, curShapePos); 
						vec3 *Nnorm = N.normalize(); //the UNIT LENGTH surface normal at the point of intersection
						vec3 **L = new vec3*[lightList.size()]; //dynamically allocated array of vec3 objects, each of which will be subsequently allocated on the heap by calling normalize (see below)
						std::deque<Light>::iterator m; //the current light: to compute the final intensity we must add up contributions from ALL lights in the scene via the following for loop:					
						for (m = lightList.begin(); m != lightList.end(); m++) //sum values for all lights m <-> the set of all lights M, where M = {m0, m1, ..., mn}
						{
							//compound rgb values for each light parameter...for usage in the intensity equation...
							ambR+=m->amb[0];
							ambG+=m->amb[1];
							ambB+=m->amb[2];

							diffR+=m->diff[0];
							diffG+=m->diff[1];
							diffB+=m->diff[2];

							L[std::distance(lightList.begin(), m)] = vec3(m->position, hitA).normalize(); //each implicit entry within L contains a normalized vector allocated ON THE HEAP due to the call to normalize()
						}

						GLdouble globalAmbient[3] = {ambR, ambG, ambB}; //global intensity of ambient light...aka sum all ambient light RGB values...
						GLdouble objAmbient[4] = {it->amb[0], it->amb[1], it->amb[2], it->amb[3]}; //object's ambient color for wavelength LAMBDA; in our case the object's R, G or B value for ambient color...
						GLdouble mTotalIntensity[3] = {(ambR+diffR), (ambG+diffG), (ambB+diffB)}; //intensity of light m for wavelength LAMBDA; in our case the R, G, or B value of the light color for light m
						GLdouble globalDiffuse[3] = {diffR, diffG, diffB}; //global diffuse coefficient
						GLdouble objDiffuse[3] = {it->diff[0], it->diff[1], it->diff[2]}; //object's diffuse color wave length LAMBDA; in our case the OBJECT's R, G or B value for diffuse color.
					
						for (int i = 0; i < lightList.size(); i++) //for each ith light in the world, compound the respective RGB values...
						{	
							finalIntensity_R += ((globalAmbient[0]*it->amb[0]) + ((mTotalIntensity[0] * (globalDiffuse[0] * it->diff[0] * Nnorm->dot(*L[i])))));
							finalIntensity_G += ((globalAmbient[1]*it->amb[1]) + ((mTotalIntensity[1] * (globalDiffuse[1] * it->diff[1] * Nnorm->dot(*L[i])))));
							finalIntensity_B += ((globalAmbient[2]*it->amb[2]) + ((mTotalIntensity[2] * (globalDiffuse[2] * it->diff[2] * Nnorm->dot(*L[i])))));
						}

						GLdouble LAMBDA[3] = {finalIntensity_R, finalIntensity_G, finalIntensity_B};

						printf("CYLINDER: two hits detected! LAMBDA for Shape #%d = {%g, %g, %g}. Raster_x = %d, Raster_y = %d\n", (int)std::distance(shapeList.begin(), it), LAMBDA[0], LAMBDA[1], LAMBDA[2], raster_x, raster_y);
						plotPixel(screen, raster_x, raster_y, LAMBDA[0], LAMBDA[1], LAMBDA[2], 1.0f);
						glBindTexture(GL_TEXTURE_2D, texId);
						glTexSubImage2D(GL_TEXTURE_2D, 0, raster_x, raster_y, 1, 1, GL_RGBA, GL_FLOAT, (void*)(screen + (raster_y*WINDOW_WIDTH+raster_x)));
						///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

						for (int i = 0; i < lightList.size(); i++)
						{
							delete L[i]; //delete each DYNAMICALLY ALLOCATED vec3 object resultant from the explicit call to each vec3's normalize() method contained within L[].
						}
						delete[] L;
						delete Nnorm;
					}
				}
				else
				{
					//std::cout << "No hits detected." << std::endl;
					//printf("No cylinder hits detected. Calculating ray originating at {%g, %g, %g} with direction {%g, %g, %g}\n", Peye.x, Peye.y, Peye.z, dir.x, dir.y, dir.z);
				}
				delete[] result;
			}
		}

		/////////////Garbage Collection////////////////////////
		delete dirNorm;
		//_CrtDumpMemoryLeaks();
		///////////////////////////////////////////////////////
 	}

	if (raster_x < WINDOW_WIDTH)
		raster_x++;

	if (raster_x == WINDOW_WIDTH) // finished line, increment Y raster
	{
		raster_x = 0;
		raster_y++;
	}

	if (raster_y > WINDOW_HEIGHT)
	{
		char inputCh;
		std::cout << "........................RENDERING COMPLETE......................\nType 'q' to exit." << std::endl;
		std::cin >> inputCh;
		if (inputCh=='q')
			exit(0);
	}

	// redraw
	// Note that this function will not be called again until the display
	// update completes. This limits the number of times this function
	// is called to you monitor refresh rate. To get around this, you
	// could calculate several lines and then update, or you can do the
	// entire thing and update.
	glutPostRedisplay();

	//delete nearPos;
	//delete farPos;
}

void resizeWindow(int width, int height)
{
	if (height == 0) // prevent div/0
		height = 1;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)width/(double)height, 0.1, 4096.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void my_TimeOut(int id)
{
	glutPostRedisplay();
	glutTimerFunc(60, my_TimeOut, 0);
}