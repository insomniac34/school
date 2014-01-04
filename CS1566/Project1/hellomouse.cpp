
/* @Programmer: Modified by Tyler Raborn, originally gem, based on hwa's
 * @Assignment: CS1566 Assignment 1
 * @Environment: Visual Studio 2012, Windows 8 Pro 64-bit, classic GLUT library installed directly into Visual Studio's Visual C/C++ lib/include directories.
 * @Description: This file provides the implementation of the main hellomouse application.
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include <list>
#include <GL/glut.h>
#include <stdio.h>
#include "hellomouse.h"
#include "Shape.h"
#include <algorithm>
#define RADIANS M_PI / 180.0

/* define and initialize some global variables */
double my_x = 0, my_y = 0; /* current position */
int window_w = 400, window_h = 300; /* window height and width */
double my_rect_w = 0.1, my_rect_h = 0.1; /* rectangle width and height */

int altFlag = 0; /*allows for alternating shapes to be printed every mouseclick*/
std::list<Shape> shapeList; //data structure for holding Shape objects
int objects = 0; //monitors # of shapes created....

int startFlag = 0;



/*Typical OpenGL/GLUT Main function */ 
int main(int argc, char **argv) { /* program arguments */

  /* initialize GLUT and OpenGL; Must be called first */
  glutInit( &argc, argv );
  
  /* our own initializations; we'll define these setup procedures */
  glut_setup();  
  gl_setup();
  my_setup();

  /* turn control over to GLUT */
  glutMainLoop() ;

  return(0) ; /* make the compiler happy */
}



/* Typical function to set up GLUT*/
void glut_setup(void) {

  /* specify display mode -- here we ask for double buffering and RGB coloring */
  glutInitDisplayMode (GLUT_DOUBLE |GLUT_RGB);

  /* make a window of size window_w by window_h; title of "GLUT Basic Interaction" placed at top left corner */
  glutInitWindowSize(window_w, window_h);
  glutInitWindowPosition(0,0);
  glutCreateWindow("It's raining balls");

  /*initialize typical callback functions */
  glutDisplayFunc( my_display );
  glutReshapeFunc( my_reshape ); 
  glutIdleFunc( my_idle );
  glutMouseFunc( my_mouse );	
  glutKeyboardFunc( my_keyboard );	
  glutTimerFunc( 200, my_TimeOut, 0);/* schedule a my_TimeOut call with the ID 0 in 20 seconds from now */
  return ;
}

/* Typical function to set up OpenGL */
/* For now, ignore the contents of function */
void gl_setup(void) {

  /* specifies a background color: black in this case */
  glClearColor(0,0,0,0); //(r, g, b, alpha) where alpha sets transparency

  /* setup for simple 2d projection */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  /* map unit square to viewport window */
  gluOrtho2D(0.0, 1.0, 0.0, 1.0); 

  return ;
}


void my_idle(void) 
{
  return;
}


void my_setup(void) 
{
  return;
}

/* drawing function */
/* TODO: draw some other shape, use some other colors */
/* TODO part 2: draw at the location of the mouse click */
void my_display(void) 
{
	objects++;
	/* clear the buffer */
	glClear(GL_COLOR_BUFFER_BIT);

	/* draw small rectangle; specify two opposing corners as arguments*/
	/* note how the rectangle coordinates we pass as arguments are mapped to the window coordinates */
	/* basically, we need to map x values between (0,window_w) to the (0,1) interval etc. */
	if (startFlag == 0) startFlag = 1;
	else
	{
		if (altFlag==1) //draw rectangle at initial positional coordinates; insert rectangle in global data structure
		{
			printf("<Shape:>Drawing rectangle at (x, y)-coordinates (%d, %d)\n", (int)my_x, (int)my_y);
			int rgbColor[3] = {0, 0, 1};
			double vertices[2] = {my_x, my_y};

			Shape* my_rectangle = new Shape(0, vertices, rgbColor); //uses constructor (shape type, list of vertex coordinates, length of coordinate list, color)
			my_rectangle->drawShape(window_w, window_h, my_rect_h, my_rect_w);
			altFlag=0;

			if (shapeList.size() < 26) 
			{
				shapeList.push_front(*my_rectangle); //add newly instantiated shape to front of linked list.	
			}
			else
			{
				printf("<Linked List:>MAXIMUM SIZE REACHED: %d REMOVING LAST ELEMENT!\n", shapeList.size());
				shapeList.pop_back(); //delete last element
				shapeList.push_front(*my_rectangle);
			}
		}
		else //draw circle at initial positional coordinates; insert circle in global data structure
		{
			printf("<Shape:>Drawing circle at (x, y)-coordinates (%d, %d)\n", (int)my_x, (int)my_y);
			int circleColor[3] = {1, 0, 0};
			double circleCenter[2] = {my_x, my_y}; //{center x coord, center y coord}

			Shape* my_circle = new Shape(1, circleCenter, circleColor);
			my_circle->drawShape(window_w, window_h, my_rect_h, my_rect_w);
			altFlag=1;

			if (shapeList.size() < 26)  
			{	
				shapeList.push_front(*my_circle); //add newly instantiated shape to linked list
			}
			else
			{
				printf("<Linked List:> MAXIMUM SIZE REACHED: %d REMOVING LAST ELEMENT!\n", shapeList.size());
				shapeList.pop_back(); //delete last element
				shapeList.push_front(*my_circle);
			}
		}
		printf("\n<Linked List:>Beginning iteration over linked list of size %d\n", (int)shapeList.size());
		for (std::list<Shape>::iterator iter = shapeList.begin(); iter != shapeList.end(); iter++) //iterate over each element in the list and draw it in it's current state.
		{
			iter->drawShape(window_w, window_h, my_rect_h, my_rect_w);
		}
	}
	/* buffer is ready; show me the money! */
	glutSwapBuffers();  
	return;
}

static void updateShapes() //called periodically by the Timer function; responsible for modifying the data contained within the shapeList data structure AND drawing the new shapes as they descend...
{
	glClear(GL_COLOR_BUFFER_BIT);
	//printf("Updating shapes...\n");
	for (std::list<Shape>::iterator iter = shapeList.begin(); iter != shapeList.end(); iter++) //for-loop goes through each shape stored in shapeList and updates it's Y coordinates
	{
		if (iter->rest != true) //if object is at rest do not call it's update method.
		{
			iter->update(window_h);
		}
		iter->drawShape(window_w, window_h, my_rect_h, my_rect_w);	
	}
	glutSwapBuffers();  
}


/* called if resize event (i.e., when we create the window and later whenever we resize the window) */ 
void my_reshape(int w, int h) {
  /* define the viewport to be the entire window */
  glViewport (0, 0, w, h); 
  window_w = w;
  window_h = h;
  return;
}


/* called if mouse event (i.e., mouse click) */
/* TODO: correct my_y so we draw exactly where the mouse clicked */
/* TODO for EC: have more fun in here */
void my_mouse(int b, int s, int x, int y) {
  switch (b) {            /* b indicates the button */
  case GLUT_LEFT_BUTTON:
    if (s == GLUT_DOWN)  {    /* button pressed */
      printf("\n<I/O:>Left button pressed at %d %d\n", x, (window_h - y));
      my_x = (double) (x); 
      my_y = (double) (window_h - y);

      my_display();

    }
    else if (s == GLUT_UP) { /* button released */
      printf("<I/O:>Left button released\n");
    }
    break;
  case GLUT_RIGHT_BUTTON:
    if (s == GLUT_DOWN)  {    /* button pressed */
      printf("\nRight button pressed\n");
    }
    else if (s == GLUT_UP) { /* button released */
      printf("Right button released\n");
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    if (s == GLUT_DOWN)  {    /* button pressed */
      printf("\nMiddle button pressed at %d %d\n", x, y);
    }
    else if (s == GLUT_UP) { /* button released */
      printf("Middle button released\n");
    }
    break;

  }
}


/* called if keyboard event (keyboard key hit) */
/* TODO for EC: do something fun in here */
void my_keyboard(unsigned char c, int x, int y) {
  switch (c) {  /* c is the key that is being hit */
  case 'c': {
    printf("C key is hit\n");
  }
    break;
   case 'y': {
    printf("Y key is hit\n");
  }
    break;
    /* and so on*/
  }

}

/* called if timer event */
/* TODO for EC: do something fun in here */
void my_TimeOut(int id) 
{
	/* id is the id of the timer; you can schedule as many timer events as you want in the callback setup section of glut_setup */

	/*...you could use this timer event to advance the state of animation incrementally */
	/* if animating you shd replace the 20 secs with something smaller, like 20 msec */
	/* but anyway, right now it only displays a benign message */

	if (startFlag != 0) updateShapes();

	//printf("\nTICK! benign timer at your service!");
 
	//glutTimerFunc(75, my_TimeOut, 0);/* schedule next timer event, 20 secs from now */
	glutTimerFunc(20, my_TimeOut, 0);/* schedule next timer event, 20 secs from now */

}
