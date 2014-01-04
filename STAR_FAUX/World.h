/*
		Written by Tyler Raborn & Charlie Koch

				this is the entrypoint into our game. As in, MAIN.

	This code is freely distributable under the GNU General Public License. See LICENSE.txt for more details.
*/

//C++ headers:
#include <deque> //Tyler: literally the best data structure ever...like an ArrayList but without that whole Java mess! ;)
#include <string>
#include <list>
#include <memory>

//C headers:
#define _USE_MATH_DEFINES	
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "stdio.h"

//MUST make sure that the Apple Magical Fairy Princess is sufficiently pleased -tyler
#ifdef __APPLE__
	#include <GLUT/glut.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else
	#include <GL/glut.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

//local headers:
#include "Light.h"
#include "Shape.h"
#include "Projectile.h"
#include "vec3.h"
#include "Vertex.h"
#include "Camera.h"
#include "NPC.h"
#include "Path.h"
#include "Shape3D.h"
#include "player.h"
#include "Billboard.h"
#include "Menu.h"
#include "SOIL/SOIL.h"
//#include "Model.h"

#define SHAPE_CUBE 0
#define SHAPE_CYLINDER 1
#define SHAPE_SPHERE 2
#define SHAPE_TORUS 3
#define SHAPE_CONE 4

#define NPC_HOSTILE 0
#define NPC_NEUTRAL 1
#define NPC_FRIENDLY 2

//OpenGL/GLUT Application Entrypoints
int main(int, char**);
void glut_setup();
void gl_setup(void);

//openGL/GLUT callback funcs
void gl_display(void);
void gl_reshape(int width, int height);
void gl_mouse(int button, int state, int mouseX, int mouseY);
void gl_mouse_drag(int mouseX, int mouseY);
void gl_keyboard(unsigned char key, int x, int y);
void gl_keyboard_up(unsigned char key, int x, int y);
void gl_special_keys(int key, int x, int y);
void gl_timer(int timerID);
void gl_idle(void);
void gl_cleanup(void);

void moveCamera(GLfloat, GLfloat, GLfloat);
void turnCamera(GLfloat, int, int, int);
void realTimeCleanup();
void playerFire();
void createStaticObjects();
void drawStaticObjects();
void animateProjectiles();
void animateThrusters();
void worldRedraw();

void spawnEnemy(GLdouble*);
void NPCupdate(GLdouble*);

void arwing_setup();
void checkTriggers();
void drawBillboards();
