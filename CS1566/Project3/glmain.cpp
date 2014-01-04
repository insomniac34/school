/*******************************************************
FILE: glmain.c
AUTHOR: Modified by Tyler Raborn, originally by gem, loosely based on some hwa
DATE: 02/09/10
DOES: skeleton for hmkw3
********************************************************/

/**********
notes:
VS users: include gl/glut.h below instead of glut.h
OSX users: include glut/glut.h below instead of glut.j
**********/

//VisS will not let through math.h constants such as M_PI w/out the line below
#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "Shape.h"
#include "matrix.h"
#include "Face.h"
#include "Vertex.h"

#define RADIANS M_PI / 180.0
#define SHAPE_CUBE 0
#define SHAPE_CYLINDER 1
#define SHAPE_SPHERE 2
#define SHAPE_TORUS 3
#define SHAPE_CONE 4
#define MAX_VS 50
#define MAX_RS 50

//#include "cs1566globals.h"
//#include "stitcher.h"
//#include "transformer.h"
#include "glmain.h"

double cyl_height=1;
double cyl_ray=1;
double sph_ray=1;
double x_camera = 0, y_camera = 5, z_camera = 30;
double x_lookAt = 0.0, y_lookAt = 0.0, z_lookAt = 0.0;

int crt_render_mode;
int crt_shape, crt_rs, crt_vs;
int crt_transform;
int spin_val = SPIN_DISABLED;
 
Shape my_cylinder;
Shape my_cone;
Shape my_sphere;
Shape my_torus;
Shape my_cube;

bool normalFlag;

GLdouble global_ax = 0.0;
GLdouble global_ay = 3.0;
GLdouble global_az = 0.0;

GLdouble global_cx = 5.0;
GLdouble global_cy = 0.0;
GLdouble global_cz = 5.0;



/*******************************************************
FUNCTION: main
ARGS: argc, argv
RETURN: 0
DOES: main function (duh!); starts GL, GLU, GLUT, then loops 
********************************************************/
int main(int argc, char** argv)

{	

  glutInit(&argc, argv);
  glut_setup();
  gl_setup();
  my_setup();

  glutMainLoop();

  return(0);

}




/*******************************************************
FUNCTION: glut_setup
ARGS: none
RETURN: none
DOES: sets up GLUT; done here because we like our 'main's tidy
********************************************************/
/* This function sets up the windowing related glut calls */
void glut_setup (){

  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
  glutInitWindowSize(700,700);
  glutInitWindowPosition(20,20);
  glutCreateWindow("Transformer");


  /* set up callback functions */
  glutDisplayFunc(my_display);
  glutReshapeFunc(my_reshape);
  glutMouseFunc(my_mouse);
  glutKeyboardFunc(my_keyboard);
  glutKeyboardUpFunc( my_keyboard_up );	
  glutTimerFunc( 20, my_TimeOut, 0 );/* schedule a my_TimeOut call with the ID 0 in 20 seconds from now */
  return;

}


/*******************************************************
FUNCTION: gl_setup
ARGS: none
RETURN: none
DOES: sets up OpenGL (name starts with gl but not followed by capital letter, so it's ours)
********************************************************/
void gl_setup(void) {
  // enable depth handling (z-buffer)
  glEnable(GL_DEPTH_TEST);

  // define the background color 
  glClearColor(0,0,0,1);

  glMatrixMode(GL_PROJECTION) ;

  glLoadIdentity() ;

  gluPerspective( 20, 1.0, 15, 100.0);

  glMatrixMode(GL_MODELVIEW) ;

  glLoadIdentity() ;  // init modelview to identity

  return ;

}


/*******************************************************
FUNCTION: my_setup
ARGS: 
RETURN:
DOES: pre-computes stuff and presets some values
********************************************************/
/*TODO add make_cone, make_torus, make_your_shape etc.   */
void my_setup(){
  crt_render_mode = GL_POLYGON;//GL_LINE_LOOP;
  crt_shape = HOUSE;
  crt_transform = NONE_MODE;
  crt_rs = 20;
  crt_vs = 10;

  normalFlag=false;

  make_cube_smart(1);
  make_cylinder(cyl_height,cyl_ray,crt_rs,crt_vs);
  make_sphere(sph_ray,crt_rs,crt_vs);
  make_torus(sph_ray, crt_rs, crt_vs);
  make_cone(cyl_height, cyl_ray, crt_rs, crt_vs);
  return;

}



/*******************************************************
FUNCTION: my_reshape
ARGS: new window width and height 
RETURN:
DOES: remaps viewport to the Window Manager's window
********************************************************/
void my_reshape(int w, int h) 
{
  // ensure a square view port
  glViewport(0,0,min(w,h),min(w,h)) ;
  return ;

}

/*******************************************************
FUNCTION: real_rev
ARGS: angle and axis x, y, z 
RETURN:
DOES: makes shape spin around its vertical axis
Note: 'vertical' axis means house keeps revolving around its roof-top
base-center axis, torus spins around axis passing through hole, 
sphere spins around North-South axis etc *at all times*.
********************************************************/
/*TODO */
void real_rev(GLfloat deg, GLfloat x, GLfloat y, GLfloat z) 
{
	//printf("Entering axis rotation mode...\n");
	//my_torus.revolve(deg, x, y, z);
	if (crt_shape==CYLINDER) 
	{
		my_cylinder.revolve(deg, x, y, z);
	}

	if (crt_shape==TORUS) 
	{
		my_torus.revolve(deg, x, y, z);
	}

	if (crt_shape==CONE) 
	{
		my_cone.revolve(deg, x, y, z);
	}

	if (crt_shape==SPHERE) 
	{
		my_sphere.revolve(deg, x, y, z);
	}

	if (crt_shape==CUBE)
	{
		my_cube.revolve(deg, x, y, z);
	}

}

/*******************************************************
FUNCTION: real_rot_arbitrary_axis_point
ARGS: angle and arbitrary axis ax, ay, az, arbitrary point cx, cy, cz 
RETURN:
DOES: makes shape spin around arbitrary axis passing through arbitrary point
Note: try a diagonal axis going through the back bottom left corner of the house
********************************************************/
/*TODO */
void real_rot_arbitrary_axis_point(GLfloat deg, GLfloat ax, GLfloat ay, GLfloat az, GLfloat cx, GLfloat cy, GLfloat cz ) 
{
	//printf("rotating about arbitrary point {%g, %g, %g}, arbitrary axis {%g, %g, %g}\n", cx, cy, cz, ax, ay, az);
	//my_torus.arbitraryRotate(deg, ax, ay, az, cx, cy, cz);
	if (crt_shape==CYLINDER) 
	{
		my_cylinder.arbitraryRotate(deg, ax, ay, az, cx, cy, cz);
	}

	if (crt_shape==TORUS) 
	{
		my_torus.arbitraryRotate(deg, ax, ay, az, cx, cy, cz);
	}

	if (crt_shape==CONE) 
	{
		my_cone.arbitraryRotate(deg, ax, ay, az, cx, cy, cz);
	}

	if (crt_shape==SPHERE) 
	{
		my_sphere.arbitraryRotate(deg, ax, ay, az, cx, cy, cz);
	}

	if (crt_shape == CUBE)
	{
		my_cube.arbitraryRotate(deg, ax, ay, az, cx, cy, cz);
	}
}



/*******************************************************
FUNCTION: real_translation
ARGS: translation amount along x, y, z
RETURN:
DOES: translates shape for real
********************************************************/
/*TODO. Note: Absolutely no gl calls */
/*can assume model-matrix stack contains at this point nothing but viewing transform*/
void real_translation(GLfloat x, GLfloat y, GLfloat z) 
{
	//printf("translating by %g, %g, %g\n", x, y, z);
	if (crt_shape==CYLINDER) 
	{
		my_cylinder.translate(x, y, z);
	}

	if (crt_shape==TORUS) 
	{
		my_torus.translate(x, y, z);
	}

	if (crt_shape==CONE) 
	{
		my_cone.translate(x, y, z);
	}

	if (crt_shape==SPHERE) 
	{
		my_sphere.translate(x, y, z);
	}

	if (crt_shape == CUBE)
	{
		my_cube.translate(x, y, z);
	}
}


/*******************************************************
FUNCTION: real_scaling
ARGS: scaling amount along x, y, z
RETURN:
DOES: scales shape, for real
********************************************************/
/*TODO */
/*can assume model-matrix stack contains at this point nothing but viewing transform*/
void real_scaling(GLfloat sx, GLfloat sy, GLfloat sz) 
{
	//my_torus.scale(sx, sy, sz);
	if (crt_shape==CYLINDER) 
	{
		my_cylinder.scale(sx, sy, sz);
	}

	if (crt_shape==TORUS) 
	{
		my_torus.scale(sx, sy, sz);
	}

	if (crt_shape==CONE) 
	{
		my_cone.scale(sx, sy, sz);
	}

	if (crt_shape==SPHERE) 
	{
		my_sphere.scale(sx, sy, sz);
	}

	if (crt_shape==CUBE)
	{
		my_cube.scale(sx, sy, sz);
	}
}

/*******************************************************
FUNCTION: real_rotation
ARGS: angle and axis
RETURN:
DOES: rotates shape, for real 
********************************************************/
/*TODO. Note: Absolutely no gl calls*/
/*can assume model-matrix stack contains at this point nothing but viewing transform */
void real_rotation(GLfloat deg, GLfloat x, GLfloat y, GLfloat z) 
{
	deg =.1;
	//printf("Rotating shape by %g degrees around coordinates {%g, %g, %g}\n", deg, x, y, z);
	if (crt_shape==CYLINDER) 
	{
		my_cylinder.rotate(deg, x, y, z);
	}

	if (crt_shape==TORUS) 
	{
		my_torus.rotate(deg, x, y, z);
	}

	if (crt_shape==CONE) 
	{
		my_cone.rotate(deg, x, y, z);
	}

	if (crt_shape==SPHERE) 
	{
		my_sphere.rotate(deg, x, y, z);
	}

	if (crt_shape==CUBE)
	{
		my_cube.rotate(deg, x, y, z);
	}
}


/*******************************************************
FUNCTION: reset
ARGS: 
RETURN:
DOES: resets shape and camera
********************************************************/
/*TODO reset shape post real transformations, as well etc*/
void reset()
{
  glMatrixMode(GL_MODELVIEW) ;
  glLoadIdentity() ;
  x_camera = 0; y_camera = 5; z_camera =30;


 	//my_cylinder = Shape(SHAPE_CYLINDER, cyl_ray, crt_rs, crt_vs, cyl_height);
	//my_cylinder.tesselate();
	make_cylinder(cyl_height, cyl_ray, crt_rs, crt_vs);

	//my_sphere = Shape(SHAPE_SPHERE, sph_ray, crt_rs, crt_vs, 3.0); 
	//my_sphere.tesselate();
	make_cone(cyl_height, cyl_ray, crt_rs, crt_vs);

	//my_torus = Shape(SHAPE_TORUS, 0.0, crt_rs, crt_vs, 3.0); 
	//my_torus.tesselate();
	make_sphere(sph_ray, crt_rs, crt_vs);

	make_torus(sph_ray, crt_rs, crt_vs);

}


/*******************************************************
FUNCTION: my_keyboard
ARGS: key id, x, y
RETURN:
DOES: handles keyboard events
********************************************************/
/*TODO: expand to add hmwk 3 keyboard events */
void my_keyboard( unsigned char key, int x, int y ) {
  switch( key ) {
  case 'O':
  case 'o': {
    reset();
    glutPostRedisplay(); 
  }; break;


	//MANIPULATE CAMERA Z:
    case 'N': {
	//printf("updating camera...\n");
    z_camera++;
	z_lookAt++;
	glutPostRedisplay();
  }; break;

	case 'n': {
	z_camera--;
	z_lookAt--;
	glutPostRedisplay();
  }; break;

	//MANIPULATE CAMERA X:
    case 'P': {
	//printf("updating camera...\n");
    x_camera++;
	x_lookAt++;
	glutPostRedisplay();
  }; break;

	case 'p': {
	x_camera--;
	x_lookAt--;
	glutPostRedisplay();
  }; break;

	//MANIPULATE CAMERA Y:
    case 'V': {
	//printf("updating camera...\n");
    y_camera++;
	y_lookAt++;
	glutPostRedisplay();
  }; break;

	case 'v': {
	y_camera--;
	y_lookAt--;
	glutPostRedisplay();
  }; break;

  case 'y': {
	switch( crt_transform) {
	case ROTATION_MODE: real_rotation(-2,0,-1,0); break;
	case TRANSLATION_MODE: real_translation(0,-1,0); break;
	case AXIS_ROTATION_MODE: real_rot_arbitrary_axis_point(-.1, global_ax, global_ay, global_az, global_cx, global_cy, global_cz); break;
	case SCALING_MODE: real_scaling(1, .8, 1); break;
	}
    glutPostRedisplay(); 
  }; break;

  case 'Y': {
	switch( crt_transform) {
	case ROTATION_MODE: real_rotation(2,0,1,0); break;
	case TRANSLATION_MODE: real_translation(0,1,0); break;
	case AXIS_ROTATION_MODE: real_rot_arbitrary_axis_point(.1, global_ax, global_ay, global_az, global_cx, global_cy, global_cz); break;
	case SCALING_MODE: real_scaling(1, 1.2, 1); break;
	}
    glutPostRedisplay(); 
  }; break;

  case 'x': {
	switch( crt_transform) {
	case ROTATION_MODE: real_rotation(-2,-1,0,0); break;
	case TRANSLATION_MODE: real_translation(-1,0,0); break;
	case AXIS_ROTATION_MODE: real_rot_arbitrary_axis_point(-.1, global_ax, global_ay, global_az, global_cx, global_cy, global_cz); break;
	case SCALING_MODE: real_scaling(.8, 1, 1); break;
	}
    glutPostRedisplay(); 
  }; break;

  case 'X': {
	switch( crt_transform) {
	case ROTATION_MODE: real_rotation(2,1,0,0); break;
	case TRANSLATION_MODE: real_translation(1,0,0); break;
	case AXIS_ROTATION_MODE: real_rot_arbitrary_axis_point(.1, global_ax, global_ay, global_az, global_cx, global_cy, global_cz); break;
	case SCALING_MODE: real_scaling(1.2, 1, 1); break;
	}
    glutPostRedisplay(); 
  }; break;

	 case 'z': {
	switch( crt_transform) {
	case ROTATION_MODE: real_rotation(-2,0,0,-1); break;
	case TRANSLATION_MODE: real_translation(0,0,-1); break;
	case AXIS_ROTATION_MODE: real_rot_arbitrary_axis_point(-.1, global_ax, global_ay, global_az, global_cx, global_cy, global_cz); break;
	case SCALING_MODE: real_scaling(1, 1, .8); break;
	}
    glutPostRedisplay(); 
  }; break;

  case 'Z': {
	switch( crt_transform) {
	case ROTATION_MODE: real_rotation(2,0,0,1); break;
	case TRANSLATION_MODE: real_translation(0,0,1); break;
	case AXIS_ROTATION_MODE: real_rot_arbitrary_axis_point(.1, global_ax, global_ay, global_az, global_cx, global_cy, global_cz); break;
	case SCALING_MODE: real_scaling(1, 1, 1.2); break;
	}
    glutPostRedisplay(); 
  }; break;

  //etc...

  case '1': {
    crt_shape = CUBE;
    glutPostRedisplay();
  }; break;
  case '0': {
    crt_shape = HOUSE;
    glutPostRedisplay();
  }; break;
  case '3':{
    crt_shape = CYLINDER;
    glutPostRedisplay();
  }; break;
  case '4':{
    crt_shape = TORUS;
    glutPostRedisplay();
  }; break;
  case '2': {
    crt_shape = SPHERE;
    glutPostRedisplay();
  }; break;
  case '6': {
    crt_shape = CUBE;
    glutPostRedisplay();
  }; break; 

    case '5': {
    crt_shape = CONE;
    glutPostRedisplay();
  }; break;


  //etc ... EC shapes from hmwk2

  case 'S':
  case 's': {
    crt_transform = SCALING_MODE;
  }; break;

  case 'T':
  case 't': {
    crt_transform = TRANSLATION_MODE;
  }; break;

  case 'R':
  case 'r': {
    crt_transform = ROTATION_MODE;
  }; break;

  case 'A':
  case 'a': {
    crt_transform = AXIS_ROTATION_MODE;
  }; break;

  


  //case 'R':
  //case 'r': {
    //crt_transform = ROTATION_MODE;
  //}; break;


 

  //add camera controls...
  //...

  //add your tessellation code from hmwk2
  case '+': {
  }; break;

  case '-': {
  }; break;

  case '>': {
  }; break;

  case '<': {
  }; break;

  case 'q': 
  case 'Q':

    exit(0) ;

    break ;	

  default: break;

  }

  

  return ;

}

/*******************************************************
FUNCTION: my_keyboard_up
ARGS: key id, x, y
RETURN:
DOES: handles keyboard up events
********************************************************/
void my_keyboard_up( unsigned char key, int x, int y ) {
	switch( key ) {
	case 'S':
	case 's': 
	case 'T':
	case 't':
	case 'R':
	case 'r': {
		crt_transform = NONE_MODE; break;
		}
	}
}




/*******************************************************
FUNCTION: my_mouse
ARGS: button, state, x, y
RETURN:
DOES: handles mouse events
********************************************************/

void my_mouse(int button, int state, int mousex, int mousey) {
  switch( button ) {	
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ) 
	{
 	 if(spin_val == SPIN_ENABLED) spin_val = SPIN_DISABLED;
	  else spin_val = SPIN_ENABLED;
	  real_rev(2, 0, 1, 0);
	  glutPostRedisplay();
    }

    if( state == GLUT_UP ) {
    }
    break ;

  case GLUT_RIGHT_BUTTON:
    if( state == GLUT_DOWN ) {
	if(crt_render_mode == GL_LINE_LOOP) crt_render_mode = GL_POLYGON;
	else crt_render_mode = GL_LINE_LOOP;
      glutPostRedisplay();
    }
    if( state == GLUT_UP ) {
    }
    break ;
  }
  
  return ;

}



/***********************************
  FUNCTION: draw_quad 
  ARGS: - a vertex array
        - 4 indices into the vertex array defining a quad face
        - an index into the color array.
  RETURN: none
  DOES:  helper drawing function; draws one quad. 
   For the normal to work out, follow left-hand-rule (i.e., counter clock
   wise) 
*************************************/
void draw_quad(GLfloat vertices[][4], int iv1, int iv2, int iv3, int iv4, int ic) {
  glBegin(crt_render_mode); 
  {
    glColor3fv(colors[ic]);
    /*note the explicit use of homogeneous coords below: glVertex4f*/
    glVertex4fv(vertices[iv1]);
    glVertex4fv(vertices[iv2]);
    glVertex4fv(vertices[iv3]);
    glVertex4fv(vertices[iv4]);
  }
  glEnd();
}

/***********************************
  FUNCTION: draw_param_quad 
  ARGS: - a 50 by 50 by 4 vertex array
        - 2 indices into the vertex array defining the top left of a quad face
        - an index into the color array.
  RETURN: none
  DOES:  helper drawing function; draws one quad. 
*************************************/
void draw_param_quad(GLfloat vertices[][50][4], int line, int col, int ic) {
  glBegin(crt_render_mode); 
  {
    glColor3fv(colors[ic]);
    /*note the explicit use of homogeneous coords below: glVertex4f*/
    glVertex4fv(vertices[line][col]);
    glVertex4fv(vertices[line+1][col]);
    glVertex4fv(vertices[line+1][col+1]);
    glVertex4fv(vertices[line][col+1]);
  }
  glEnd();
}



/***********************************
  FUNCTION: draw_triangle 
  ARGS: - a vertex array
        - 3 indices into the vertex array defining a triangular face
        - an index into the color array.
  RETURN: none
  DOES:  helper drawing function; draws one triangle. 
   For the normal to work out, follow left-hand-rule (i.e., ccw)
*************************************/
void draw_triangle(GLfloat vertices[][4], int iv1, int iv2, int iv3, int ic) {
  glBegin(crt_render_mode); 
  {
    glColor3fv(colors[ic]);
    /*note the explicit use of homogeneous coords below: glVertex4f*/
    glVertex4fv(vertices[iv1]);
    glVertex4fv(vertices[iv2]);
    glVertex4fv(vertices[iv3]);
  }
  glEnd();
}


/***********************************
  FUNCTION: draw_house 
  ARGS: none
  RETURN: none
  DOES: helper build function; 
        defines a hollow cube with a bottomless pyramid as rooftop
************************************/
void draw_house()
{
  draw_triangle(vertices_house,0,1,2,RED);
  draw_triangle(vertices_house,0,2,3,GREEN);
  draw_triangle(vertices_house,0,3,4,WHITE);
  draw_triangle(vertices_house,0,4,1,GREY);


  draw_quad(vertices_house,2,1,5,6, BLUE);
  draw_triangle(vertices_house,2,6,3, CYAN);
  draw_triangle(vertices_house,3,6,7, CYAN);
  draw_triangle(vertices_house,3,7,8, YELLOW);
  draw_triangle(vertices_house,8,3,4, YELLOW);
  draw_triangle(vertices_house,4,8,1, MAGENTA);
  draw_triangle(vertices_house,1,8,5, MAGENTA);

}

/*************************************
FUNCTION: make_*; reuse your stitcher code here.
*************************************/

void make_cylinder(double height, double ray, int rs, int vs) 
{
	my_cylinder = Shape(SHAPE_CYLINDER, ray, rs, vs, cyl_height);
	my_cylinder.tesselate();
}

void make_cone(double height, double ray, int rs, int vs)
{
	printf("Entering make_cone\n...");
	my_cone = Shape(SHAPE_CONE, ray, rs, vs, cyl_height);
	my_cone.tesselate();
}

void make_sphere(double ray, int rs, int vs) 
{
	my_sphere = Shape(SHAPE_SPHERE, ray, rs, vs, 3.0); 
	my_sphere.tesselate();
}

void make_torus(double ray, int rs, int vs)
{
	my_torus = Shape(SHAPE_TORUS, ray, rs, vs, 3.0); 
	my_torus.tesselate();
}

void make_cube_smart(double size)
{
	my_cube = Shape(SHAPE_CUBE, sph_ray, crt_rs, crt_vs, size);
	my_cube.tesselate(); //DO NOT UNCOMMENT FOR BAD THINGS WILL HAPPEN
}



//SHAPE-DRAWING FUNCTIONS:
void draw_cylinder()
{
	my_cylinder.draw(crt_rs, crt_vs, crt_render_mode, normalFlag);
}

void draw_cone()
{
	my_cone.draw(crt_rs, crt_vs, crt_render_mode, normalFlag);
}

void draw_torus()
{
	my_torus.draw(crt_rs, crt_vs, crt_render_mode, normalFlag);
}

void draw_sphere()
{
	my_sphere.draw(crt_rs, crt_vs, crt_render_mode, normalFlag);
}

void draw_cube()
{
	my_cube.draw(crt_rs, crt_vs, crt_render_mode, normalFlag);
}











/***********************************
  FUNCTION: draw_axes
  ARGS: none
  RETURN: none
  DOES: draws main X, Y, Z axes
************************************/
void draw_axes( void ) {
    glLineWidth( 5.0 );
    glBegin(GL_LINES); 
  {
    glColor3fv(colors[1]);
    //glVertex4fv(vertices_axes[0]);
   // glVertex4fv(vertices_axes[1]);

  //  glColor3fv(colors[4]);
  //  glVertex4fv(vertices_axes[0]);
  //  glVertex4fv(vertices_axes[2]);

    glColor3fv(colors[6]);
  //  glVertex4fv(vertices_axes[0]);
  //  glVertex4fv(vertices_axes[3]);
  }
	glEnd();
	glLineWidth( 1.0 );

}

/***********************************
  FUNCTION: draw_object 
  ARGS: shape to create (HOUSE, CUBE, CYLINDER, etc)
  RETURN: none
  DOES: draws an object from triangles
************************************/
/*TODO: expand according to assignment 3 specs*/
void draw_object(int shape) {

  switch(shape){

  case HOUSE: 
	  draw_house();
	  break;

  case CUBE:
	  draw_cube();
	  break;
  case CYLINDER: 
	  draw_cylinder();
	  break;
  case SPHERE: 
	  draw_sphere();
	  break;
  case TORUS: 
	  draw_torus(); 
	  break;
  case CONE:
	  draw_cone();
	  break;
  case MESH: ; 
	  break;
    //case CONE: ; break;

  default: break;
  }

}


/***********************************
  FUNCTION: my_display
  ARGS: none
  RETURN: none
  DOES: main drawing function
************************************/
/*TODO add on*/
void my_display() {

  // clear all pixels, reset depth 
  glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT );
  
   // init to identity 

   glLoadIdentity() ;

   gluLookAt(x_camera, y_camera, z_camera,  // x,y,z coord of the camera 
	    x_lookAt, y_lookAt, z_lookAt,  // x,y,z LookAt
	    0.0, 1.0, 0.0); // the direction of Up (default is y-axis)

   draw_axes();


   draw_object(crt_shape);

  // this buffer is ready

  glutSwapBuffers();

}

/***********************************
  FUNCTION: my_TimeOut
  ARGS: timer id
  RETURN: none
  DOES: handles "timer" events
************************************/
void my_TimeOut(int id) {
  /* spin counter-clockwise*/
  /* schedule next timer event, 0.2 secs from now */ 
  if(spin_val == SPIN_ENABLED){
    real_rev(3,0,1,0);
    
	glutPostRedisplay();
	
  };
  glutTimerFunc(20, my_TimeOut, 0);

  return ;
}


/***********************************
  FUNCTION: my_idle
  ARGS: none
  RETURN: none
  DOES: handles the "idle" event
************************************/
void my_idle(void) {
}


