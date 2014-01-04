/**************************************************************************
File: glmain.c
Does: basic lighting and modeling for cs1566 hw4 Modeler
Author: Steven Lauck, based on some hwa
Date: 01/08/09
**************************************************************************/

#include "glmain.h"
#include <cstdio>
#include <cstdlib>
#include "Camera.h"

#define SHAPE_CUBE 0
#define SHAPE_CYLINDER 1
#define SHAPE_SPHERE 2
#define SHAPE_TORUS 3
#define SHAPE_CONE 4

#define my_assert(X,Y) ((X)?(void) 0:(printf("error:%s in %s at %d", Y, __FILE__, __LINE__), myabort()))

#define min(a,b) ((a) < (b)? a:b)
//#define FALSE 0 
//#define TRUE  1
#define MAX_LIGHTS  8
#define NUM_OBJECTS 8
#define cyl_height 1.0

//+/- modifiers
#define NEAR_CLIPPING_MOD 20
#define FAR_CLIPPING_MOD 21
#define ZOOM_MOD 22

typedef struct _Object 
{
  int sid;

  // example object storage arrays for vertex and normals
  GLfloat vertices_cube_smart[8][4];
  GLfloat normals_cube_smart[8][3];
 
  GLfloat shine;
  GLfloat emi[4];
  GLfloat amb[4];
  GLfloat diff[4];
  GLfloat spec[4];

  GLfloat translate[4];
  GLfloat scale[4];
  GLfloat rotate[4];

}OBJECT;

typedef struct _CAM{
  GLfloat pos[4];
  GLfloat at[4];
  GLfloat up[4];

  GLfloat dir[4];
}CAM;

typedef struct _LITE{
  GLfloat amb[4];
  GLfloat diff[4];
  GLfloat spec[4];
  GLfloat pos[4];
  GLfloat dir[3];
  GLfloat angle;
}LITE;

GLfloat colors [][3] = {
  {0.0, 0.0, 0.0},  /* black   */
  {1.0, 0.0, 0.0},  /* red     */
  {1.0, 1.0, 0.0},  /* yellow  */
  {1.0, 0.0, 1.0},  /* magenta */
  {0.0, 1.0, 0.0},  /* green   */
  {0.0, 1.0, 1.0},  /* cyan    */
  {0.0, 0.0, 1.0},  /* blue    */
  {0.5, 0.5, 0.5},  /* 50%grey */
  {1.0, 1.0, 1.0}   /* white   */
};

GLfloat vertices_axes[][4] = {
  {0.0, 0.0, 0.0, 1.0},  /* origin */ 
  {5.0, 0.0, 0.0, 1.0},  /* maxx */ 
  {0.0, 5.0, 0.0, 1.0}, /* maxy */ 
  {0.0, 0.0, 5.0, 1.0}  /* maxz */ 

};

void make_cube_smart(OBJECT *po, double size );

void real_translation(OBJECT *po, GLfloat x, GLfloat y, GLfloat z);
void real_scaling(OBJECT *po, GLfloat sx, GLfloat sy, GLfloat sz);
void real_rotation(OBJECT *po, GLfloat deg, GLfloat x, GLfloat y, GLfloat z);

void translateCamera(GLdouble, GLdouble, GLdouble);
void translateCameraLocalized(GLdouble, GLdouble, GLdouble);
void rotateCamera(GLdouble, GLdouble, GLdouble, GLdouble);
void rotateCameraLocalized(GLdouble, GLdouble, GLdouble, GLdouble);

void cameraDecreaseHeight();
void cameraIncreaseHeight();
void decreaseFCP();
void increaseFCP();
void decreaseNCP();
void increaseNCP();
void cameraZoomIn();
void cameraZoomOut();
void aspectRatioDecrease();
void aspectRatioIncrease();

void camPrint();



OBJECT my_objects[NUM_OBJECTS];
LITE my_lights[MAX_LIGHTS];
int num_objects;
int  num_lights;
GLfloat GLOBAL_COLOR = BLUE;


int camFlag;

// camera variables
CAM my_cam;
GLfloat camx, camy, camz;
GLfloat atx, aty, atz;
GLfloat upx, upy, upz;

Camera myCamera;

GLdouble GLOBAL_FAR = 200.0;


//TRUE or FALSE
int firstPersonView;

int crt_render_mode;
int crt_shape, crt_rs, crt_vs;
int crt_transform;
int cam_mod;

void myabort(void) {
  abort();
  exit(1); /* exit so g++ knows we don't return. */
} 

int main(int argc, char** argv)
{ 
  //my_shapes_idx = 0;
  camFlag=0;
  setbuf(stdout, NULL);   /* for writing to stdout asap */
  glutInit(&argc, argv);

  my_setup(argc, argv);  
  glut_setup();
  gl_setup();

  glutMainLoop();
  return(0);
}


void glut_setup (){

  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
  
  glutInitWindowSize(700,700);
  glutInitWindowPosition(20,20);
  glutCreateWindow("CS1566 Project 4");

  /* set up callback functions */
  glutDisplayFunc(my_display);
  glutReshapeFunc(my_reshape);
  glutMouseFunc(my_mouse);
  glutMotionFunc(my_mouse_drag);
  glutKeyboardFunc(my_keyboard);
  glutKeyboardUpFunc( my_keyboard_up ); 
  glutIdleFunc( my_idle );  

  return;
}

void gl_setup(void) {

  // enable depth handling (z-buffer)
  glEnable(GL_DEPTH_TEST);

  // enable auto normalize
  glEnable(GL_NORMALIZE);

  // define the background color 
  glClearColor(0,0,0,1);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  myCamera.align(); 
  myCamera.transform(); 

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  //gl_setup();


  if (camFlag == 0)
  {
    //gluPerspective( 40, 1.0, 1, 200.0);
    myCamera.scalePerspective(10, 1.0, 1, 200.0);
  } 
  else
  {
    myCamera.scale(200, 10, 10, 1);
  } 

  glMatrixMode(GL_MODELVIEW);

  // toggle to smooth shading (instead of flat)
  //glShadeModel(GL_SMOOTH); 
  //lighting_setup();
  return ;
}

void my_setup(int argc, char **argv)
{

  firstPersonView = 1;
  num_objects = num_lights = 0;

  // initialize global shape defaults and mode for drawing
  crt_render_mode = GL_LINE_LOOP;
  crt_shape = 0;

  crt_rs = 40;
  crt_vs = 40; 
 
  //If you want to allow the user to type in the spec file
  //then modify the following code.
  //Otherwise, the program will attempt to load the file as specified
  //on the command line:
  //EX: ./glmain spec3
  my_assert(argc >1, "need to supply a spec file");
  read_spec(argv[1]);
  return;
}

void parse_floats(char *buffer, GLfloat nums[]) {
  int i;
  char *ps;

  ps = strtok(buffer, " ");
  for (i=0; ps; i++) {
    nums[i] = atof(ps);
    ps = strtok(NULL, " ");
    //printf("read %f ",nums[i]);
  }

}



//parse_obj 
void parse_obj(char *buffer)
{
  OBJECT *po;
  char *pshape, *pshine, *pemi, *pamb, *pdiff, *pspec, *ptranslate, *pscale, *protate;

  my_assert ((num_objects < NUM_OBJECTS), "too many objects");
  po = &my_objects[num_objects++];

  pshape  = strtok(buffer, " "); //moves along grabbing each token delimited by " "
  //printf("pshape is %s\n",pshape);

  ptranslate    = strtok(NULL, "()");  strtok(NULL, "()");
  pscale        = strtok(NULL, "()");  strtok(NULL, "()"); 
  protate       = strtok(NULL, "()");  strtok(NULL, "()");  

  pshine  = strtok(NULL, "()");strtok(NULL, "()");
  //printf("pshine is %s\n",pshine);
 
  pemi    = strtok(NULL, "()");  strtok(NULL, "()"); 
  pamb    = strtok(NULL, "()");  strtok(NULL, "()"); 
  pdiff   = strtok(NULL, "()");  strtok(NULL, "()"); 
  pspec   = strtok(NULL, "()");  strtok(NULL, "()"); 


  po->sid  = atoi(pshape);
  po->shine = atof(pshine);

  parse_floats(ptranslate, po->translate);
  parse_floats(pscale, po->scale);
  parse_floats(protate, po->rotate);

  parse_floats(pemi, po->emi);
  parse_floats(pamb, po->amb);
  parse_floats(pdiff, po->diff);
  parse_floats(pspec, po->spec);

  // use switch to create your objects, cube given as example
  switch (po->sid){
  case 1: //cube
    printf("Constructing Cube...\n");
    //make_cube_smart(po, 1);
    my_shapes.push_front(Shape(SHAPE_CUBE, 0, 0, 0, 1.0));
    my_shapes[0].tessellate();
    break;

  case 2: //house
    printf("Constructing House...\n");
    //make_cube_smart(po, 1);
    my_shapes.push_front(Shape(SHAPE_CUBE, 0, 0, 0, 1.0));
    my_shapes[0].tessellate();
    break;

  case 3: //sphere
    printf("Constructing Sphere...\n");
    my_shapes.push_front(Shape(SHAPE_SPHERE, 0.0, crt_rs, crt_vs, 1.0));
    my_shapes[0].tessellate();
    break;

  case 4: //cylinder
    printf("Constructing Cylinder...\n");
    my_shapes.push_front(Shape(SHAPE_CYLINDER, 0.0, crt_rs, crt_vs, cyl_height));
  my_shapes[0].tessellate();
    break;

  case 5: //cone
    printf("Constructing Cone...\n");
    my_shapes.push_front(Shape(SHAPE_CONE, 0.0, crt_rs, crt_vs, cyl_height));
  my_shapes[0].tessellate();
    break;

  case 6: //torus
    printf("Constructing Torus...\n");
  my_shapes.push_front(Shape(SHAPE_TORUS, 0.0, crt_rs, crt_vs, 1.0)); 
  my_shapes[0].tessellate();
    break;
  }
  
  // scale, rotate, translate using your real tranformations from assignment 3 depending on input from spec file
  
  real_scaling(po, po->scale[0], po->scale[1], po->scale[2]);  
  real_rotation(po, po->rotate[0], 1, 0, 0);
  real_rotation(po, po->rotate[1], 0, 1, 0);
  real_rotation(po, po->rotate[2], 0, 0, 1);
  real_translation(po, po->translate[0], po->translate[1], po->translate[2]);
  
  printf("read object\n");
}




void parse_camera(char *buffer)
{
  CAM *pc;
  char *ppos, *plook, *pup;

  pc = &my_cam;

  strtok(buffer, "()");
  ppos  = strtok(NULL, "()");  strtok(NULL, "()"); 
  plook  = strtok(NULL, "()");  strtok(NULL, "()"); 
  pup  = strtok(NULL, "()");  strtok(NULL, "()"); 

  parse_floats(ppos, pc->pos);
  parse_floats(plook, pc->at);
  parse_floats(pup, pc->up);

  //GLdouble CP_far = 200.0;
  //GLdouble camera_height = 40.0;
  //GLdouble camera_width = 40.0;
  
  //initialize camera constructor


  pc->at[0] += pc->pos[0];
  pc->at[1] += pc->pos[1];
  pc->at[2] += pc->pos[2];

  pc->dir[0] = pc->at[0] - pc->pos[0];
  pc->dir[1] = pc->at[1] - pc->pos[1];
  pc->dir[2] = pc->at[2] - pc->pos[2];
  normalize(pc->dir);
  printf("read camera\n");

  myCamera = Camera(
                  pc->pos[0], pc->pos[1], pc->pos[2], 
                  pc->dir[0], pc->dir[1], pc->dir[2], 
                  pc->up[0], pc->up[1], pc->up[2],
                  my_cam.pos[0],my_cam.pos[1], my_cam.pos[2]);
  /*glLoadIdentity();


  myCamera.transform(my_cam.pos[0],my_cam.pos[1], my_cam.pos[2]);

  //STEP 2.) Align the camera's local coordinate system with the world's
  //glLoadIdentity();
  myCamera.align();*/

  



}




void parse_light(char *buffer){
  LITE *pl;
  char *pamb, *pdiff, *pspec, *ppos, *pdir, *pang;
  my_assert ((num_lights < MAX_LIGHTS), "too many lights");
  pl = &my_lights[++num_lights];

  strtok(buffer, "()");
  pamb  = strtok(NULL, "()");  strtok(NULL, "()"); 
  pdiff = strtok(NULL, "()");  strtok(NULL, "()"); 
  pspec = strtok(NULL, "()");  strtok(NULL, "()"); 
  ppos  = strtok(NULL, "()");  strtok(NULL, "()"); 
  pdir  = strtok(NULL, "()");  strtok(NULL, "()"); 
  pang  = strtok(NULL, "()");

  parse_floats(pamb, pl->amb);
  parse_floats(pdiff, pl->diff);
  parse_floats(pspec, pl->spec);
  parse_floats(ppos, pl->pos);
  if (pdir) {
    parse_floats(pdir, pl->dir);
    pl->angle = atof(pang);
    //printf("angle %f\n", pl->angle);
  }
  else
    pl->dir[0]= pl->dir[1]= pl->dir[2] =0;
  printf("read light\n");

}




//FILE IO//

void read_spec(char *fname) 
{
  char buffer[300];
  FILE *fp;

  fp = fopen(fname, "r"); //opens file for reading
  my_assert(fp, "can't open spec");
  while(!feof(fp)){
    fgets(buffer, 300, fp);
    //printf("read line: %s\n", buffer);
    switch (buffer[0]) {
    case '#':
      break;

////////SHAPETYPE READ-IN///////////
    case '1': //cube
      //printf("CUBE!\n");
        //read in the cube
      parse_obj(buffer);
      break;

    case '2': //house
      //read in the cube
      parse_obj(buffer);
      break;

    case '3': //Sphere
      //read in the cube
      parse_obj(buffer);
    //my_shapes.push_front(Shape(SHAPE_SPHERE, 0.0, crt_rs, crt_vs, 3.0));
    //my_shapes[0].translate();
      break;

    case '4': //Cylinder
      //read in the cube
      parse_obj(buffer);
      break;

    case '5': //Cone
      //read in the cube
      parse_obj(buffer);
      break;

    case '6': //Torus
      //read in the cube
      parse_obj(buffer);
      break;



    case 'l':
      parse_light(buffer);
      break;

    case 'c':
      parse_camera(buffer);
      break;

    default:
      break;
    }
  }
}

void my_keyboard_up( unsigned char key, int x, int y ) 
{
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



void lighting_setup () 
{
  int i;
  GLfloat globalAmb[]     = {.1, .1, .1, .1};

  // create flashlight
  GLfloat amb[] = {0.2, 0.2, 0.2, 1.0};
  GLfloat dif[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat spec[] = {5.0, 5.0, 5.0, 1.0};

  //enable lighting
  glEnable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);

  // reflective propoerites -- global ambiant light
  //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

  // this was for the flashlights

 // glLightfv(GL_LIGHT0, GL_POSITION, my_cam.at);
 // glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, my_cam.dir);

 // glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
 // glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
 // glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
 // glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);

 // glEnable(GL_LIGHT0);

  // setup properties of lighting
  for (i=1; i<=num_lights; i++) {
    glEnable(GL_LIGHT0+i);
    glLightfv(GL_LIGHT0+i, GL_AMBIENT, my_lights[i].amb);
    glLightfv(GL_LIGHT0+i, GL_DIFFUSE, my_lights[i].diff);
    glLightfv(GL_LIGHT0+i, GL_SPECULAR, my_lights[i].spec);
    glLightfv(GL_LIGHT0+i, GL_POSITION, my_lights[i].pos);
    if ((my_lights[i].dir[0] > 0) ||  (my_lights[i].dir[1] > 0) ||  (my_lights[i].dir[2] > 0)) {
      glLightf(GL_LIGHT0+i, GL_SPOT_CUTOFF, my_lights[i].angle);
      glLightfv(GL_LIGHT0+i, GL_SPOT_DIRECTION, my_lights[i].dir);
    }
  }

}

void my_reshape(int w, int h) {
  // ensure a square view port
  glViewport(0,0,min(w,h),min(w,h)) ;
  return ;
}

//For the following 3 functions, do with them as you like.
//You will have to somehow rotate / translate / scale\
//the object as specified in the spec file.

void my_rev(GLfloat deg, GLfloat x, GLfloat y, GLfloat z) {
 
}

void my_trans(GLfloat x, GLfloat y, GLfloat z) {

}

void my_scale(GLfloat x, GLfloat y, GLfloat z) {

}

// reuse code from assignment 3 here to do transformations when reading spec
void real_translation(OBJECT *po, GLfloat x, GLfloat y, GLfloat z) 
{
  printf("translating shape...\n");
  my_shapes[0].translate(x, y, z);
}

void real_scaling(OBJECT *po, GLfloat sx, GLfloat sy, GLfloat sz) 
{
  printf("scaling shape...\n");
  my_shapes[0].scale(sx, sy, sz);
}

void real_rotation(OBJECT *po, GLfloat deg, GLfloat x, GLfloat y, GLfloat z) 
{
  printf("rotating shape...\n");
  my_shapes[0].rotate(deg, x, y, z);
}



void my_keyboard( unsigned char key, int x, int y ) {
  OBJECT *ph;
  
  switch( key ) {
  case ' ':
    //code to switch between first person view and back as appropriate
    glutPostRedisplay();
    break;

  case 'h': {
    cameraDecreaseHeight(); 
    glutPostRedisplay();
  }; break;

  case 'H': {
    cameraIncreaseHeight(); 
    glutPostRedisplay();
  }; break;

  case 'A': {
    aspectRatioIncrease(); 
    glutPostRedisplay();
  }; break;

  case 'a': {
    aspectRatioDecrease(); 
    glutPostRedisplay();
  }; break;


  case 'P':
  case 'p': {
    camPrint();
  }; break;

  case 'N':
  case 'n': {
    cam_mod = NEAR_CLIPPING_MOD;
  }; break;

  case 'F':
  case 'f': {
    cam_mod = FAR_CLIPPING_MOD;
  }; break;

  case 'L':
  case 'l': {
    cam_mod = ZOOM_MOD;
  }; break;



  case 'T':
  case 't': {
    crt_transform = TRANSLATION_MODE;
  }; break;

  case 'S':
  case 's': {
    crt_transform = SCALING_MODE;
  }; break;

  case 'R':
  case 'r': {
    crt_transform = ROTATION_MODE;
  }; break;

  case 'c':
    if (camFlag==0) camFlag=1;
    else camFlag=0;
    printf("C pressed!\n");
    glutPostRedisplay();
    break;

  case 'u': {
  switch( crt_transform) {
  case TRANSLATION_MODE: translateCameraLocalized(0, -1, 0); break;
  }
    glutPostRedisplay(); 
  }; break;

  case 'U': {
  switch( crt_transform) {
  case TRANSLATION_MODE: translateCameraLocalized(0, 1, 0); break;
  }
    glutPostRedisplay(); 
  }; break;

  case 'v': {
  switch( crt_transform) {
  case TRANSLATION_MODE: translateCameraLocalized(-1, 0, 0); break;
  }
    glutPostRedisplay(); 
  }; break;

  case 'V': {
  switch( crt_transform) {
  case TRANSLATION_MODE: translateCameraLocalized(1, 0, 0); break;
  }
    glutPostRedisplay(); 
  }; break;

  case 'w': {
  switch( crt_transform) {
  case TRANSLATION_MODE: translateCameraLocalized(0, 0, -1); break;
  }
    glutPostRedisplay(); 
  }; break;

  case 'W': {
  switch( crt_transform) {
  case TRANSLATION_MODE: translateCameraLocalized(0, 0, 1); break;
  }
    glutPostRedisplay(); 
  }; break;

  case 'y': {
  switch( crt_transform) {
  case ROTATION_MODE: rotateCameraLocalized(-.25,0,-1,0); break;
  case TRANSLATION_MODE: translateCamera(0,-1,0); break;
  case AXIS_TRANSLATION_MODE: translateCameraLocalized(0, -1, 0); break;
  //case AXIS_ROTATION_MODE: real_rot_arbitrary_axis_point(-.1, global_ax, global_ay, global_az, global_cx, global_cy, global_cz); break;
  //case SCALING_MODE: real_scaling(ph, 1, .8, 1); break;
  }
    glutPostRedisplay(); 
  }; break;

  case 'Y': {
  switch( crt_transform) {
  case ROTATION_MODE: rotateCameraLocalized(.25,0,1,0); break;
  case TRANSLATION_MODE: translateCamera(0,1,0); break;
  case AXIS_TRANSLATION_MODE: translateCameraLocalized(0,1,0); break;
  //case AXIS_ROTATION_MODE: real_rot_arbitrary_axis_point(.1, global_ax, global_ay, global_az, global_cx, global_cy, global_cz); break;
  //case SCALING_MODE: real_scaling(ph, 1, 1.2, 1); break;
  }
    glutPostRedisplay(); 
  }; break;

  case 'x': 
  {
  switch( crt_transform) {
  case ROTATION_MODE: rotateCameraLocalized(-.25,-1,0,0); break;
  case TRANSLATION_MODE: translateCamera(-1,0,0); break;
  case AXIS_TRANSLATION_MODE: translateCameraLocalized(-1, 0, 0); break;
  //case AXIS_ROTATION_MODE: real_rot_arbitrary_axis_point(-.1, global_ax, global_ay, global_az, global_cx, global_cy, global_cz); break;
  //case SCALING_MODE: real_scaling(ph, .8, 1, 1); break;
  }
    glutPostRedisplay(); 
  }; break;

  case 'X': {
  switch( crt_transform) {
  case ROTATION_MODE: rotateCameraLocalized(.25,1,0,0); break;
  case TRANSLATION_MODE: translateCamera(1,0,0); break;
  case AXIS_TRANSLATION_MODE: translateCameraLocalized(1,0,0); break;
  //case AXIS_ROTATION_MODE: real_rot_arbitrary_axis_point(.1, global_ax, global_ay, global_az, global_cx, global_cy, global_cz); break;
  //case SCALING_MODE: real_scaling(ph, 1.2, 1, 1); break;
  }
    glutPostRedisplay(); 
  }; break;

   case 'z': {
  switch( crt_transform) {
  case ROTATION_MODE: rotateCameraLocalized(-.25,0,0,-1); break;
  case TRANSLATION_MODE: translateCamera(0,0,-1); break;
  case AXIS_TRANSLATION_MODE: translateCameraLocalized(0,0,-1); break;
  //case AXIS_ROTATION_MODE: real_rot_arbitrary_axis_point(-.1, global_ax, global_ay, global_az, global_cx, global_cy, global_cz); break;
  //case SCALING_MODE: real_scaling(ph, 1, 1, .8); break;
  }
    glutPostRedisplay(); 
  }; break;

  case 'Z': {
  switch( crt_transform) {
  case ROTATION_MODE: rotateCameraLocalized(.25,0,0,1); break;
  case TRANSLATION_MODE: translateCamera(0,0,1); break;
  case AXIS_TRANSLATION_MODE: translateCameraLocalized(0,0,1); break;
  //case AXIS_ROTATION_MODE: real_rot_arbitrary_axis_point(.1, global_ax, global_ay, global_az, global_cx, global_cy, global_cz); break;
  //case SCALING_MODE: real_scaling(ph, 1, 1, 1.2); break;
  }
    glutPostRedisplay(); 
  }; break;


  case '+': {
  switch(cam_mod) {
  case ZOOM_MOD: cameraZoomIn(); break;
  case NEAR_CLIPPING_MOD: increaseNCP(); break;
  case FAR_CLIPPING_MOD: increaseFCP(); break;
  }
    glutPostRedisplay(); 
  }; break;

  case '-': {
  switch(cam_mod) {
  case ZOOM_MOD: cameraZoomOut(); break;
  case NEAR_CLIPPING_MOD: decreaseNCP(); break;
  case FAR_CLIPPING_MOD: decreaseFCP(); break;
  }
    glutPostRedisplay(); 
  }; break;



  case 'q': 
  case 'Q':
    exit(0) ;
    break ; 
  default: break;
  }
  
  return ;
}

void my_mouse_drag(int x, int y) {
}

int my_raytrace_cube(OBJECT *s, float *rayStart, float *rayDirection, float result[3])
{
  // If there is an intersection, this should set result to the closest one to the ray start.

  // should return non-zero there is an intersection

  return 0; 
}

void my_raytrace(int mousex, int mousey)
{
  double modelViewMatrix[16];
  double projMatrix[16];
  int viewport[4];
  int foundIntersection = 0;
  int hit = 0;
  int i;
  double clickPoint[3];
  float intersectionPoint[3];
  float closestPoint[3];
  float rayStart[3];
  float rayDirection[3];
  OBJECT *cur;

  // first we need to get the modelview matrix, the projection matrix, and the viewport
  glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
  glGetIntegerv(GL_VIEWPORT, viewport);

  // gluUnProject with a Z value of 1 will find the point on the far clipping plane
  // corresponding the the mouse click. This is not the same as the vector
  // representing the click.
  gluUnProject(mousex, mousey, 1.0, modelViewMatrix, projMatrix, viewport, &clickPoint[0], &clickPoint[1], &clickPoint[2]);

  // Now we need a vector representing the click. It should start at the camera
  // position. We can subtract the click point, we will get the vector

  /* code for finding direction vector, set rayStart and rayDirection */

  // now go through the shapes and see if there is a hit
  for (i=0; i<num_objects; i++)
  {
    cur = my_objects + i;
    hit = 0;

    switch (cur->sid)
    {
    case 1:
      hit = my_raytrace_cube(cur, rayStart, rayDirection, intersectionPoint);
      break;
    default:
      break;
    }

    // found intersection
    if (hit)
    {
      if (foundIntersection)
      {
        // check to see if it is the closer than the closest one we found,
        // and if it is, store it in closestPoint
      }

      else
      {
        closestPoint[0] = intersectionPoint[0];
        closestPoint[1] = intersectionPoint[1];
        closestPoint[2] = intersectionPoint[2];
      }

      foundIntersection = 1;
    }
  }

  if (foundIntersection)
  {
    printf("Intersected with object %s at (%f, %f, %f)\n", "object_name", closestPoint[0], closestPoint[1], closestPoint[2]);
  }
  else
  {
    printf("No intersection\n");
  }
}

void my_mouse(int button, int state, int mousex, int mousey) {

  switch( button ) {
  
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ) {
    
    my_raytrace(mousex, mousey);
    }
    
    if( state == GLUT_UP ) {
    }
    break ;

  case GLUT_RIGHT_BUTTON:
    if ( state == GLUT_DOWN ) {
    }
    
    if( state == GLUT_UP ) {
    }
    break ;
  }
  

  
  return ;
}

float dotprod(float v1[], float v2[]) {
  float tot = 0;
  int i;
  for (i=0; i<4; i++)
    tot += v1[i]*v2[i];
  return tot;
}


void normalize(GLfloat *p) { 
  double d=0.0;
  int i;
  for(i=0; i<3; i++) d+=p[i]*p[i];
  d=sqrt(d);
  if(d > 0.0) for(i=0; i<3; i++) p[i]/=d;
}


void cross(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d) { 
  d[0]=(b[1]-a[1])*(c[2]-a[2])-(b[2]-a[2])*(c[1]-a[1]);
  d[1]=(b[2]-a[2])*(c[0]-a[0])-(b[0]-a[0])*(c[2]-a[2]);
  d[2]=(b[0]-a[0])*(c[1]-a[1])-(b[1]-a[1])*(c[0]-a[0]);
  normalize(d);
}

void print_matrix(float my_matrix[])
{ 
  int i, j;

  for (i=0; i<4; i++) {
    for (j=0; j<4; j++) {
      printf ("%f ", my_matrix[i+j*4]);
    }
    printf ("\n");
  }
  printf ("\n");
}

void make_quad(GLfloat vertices[][3]) {
  glBegin(GL_LINE_LOOP); 
  {
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[3]);
  }
  glEnd();
}

/*************************************
 FUNCTION: make_*; reuse your stitcher code here.
*************************************/
void make_cube_smart(OBJECT *po, double size)
{
  int i;  
  // compute verts on PI/4 angels for x y z, then -x y z  
  for(i = 0; i < 4; i++)
  {
    po->vertices_cube_smart[i][0] = size*cos((M_PI/4));
    po->vertices_cube_smart[i][1] = -size*sin(i*(M_PI/2)+(M_PI/4));
    po->vertices_cube_smart[i][2] = size*cos(i*(M_PI/2)+(M_PI/4));
    po->vertices_cube_smart[i][3] = 1;
    // mirror on x axis
    po->vertices_cube_smart[i+4][0] = -size*cos((M_PI/4));
    po->vertices_cube_smart[i+4][1] = -size*sin(i*(M_PI/2)+(M_PI/4));
    po->vertices_cube_smart[i+4][2] = size*cos(i*(M_PI/2)+(M_PI/4));
    po->vertices_cube_smart[i+4][3] = 1;
  }

  //compute normals
    cross(po->vertices_cube_smart[0], po->vertices_cube_smart[1], po->vertices_cube_smart[2], po->normals_cube_smart[0]);
    cross(po->vertices_cube_smart[1], po->vertices_cube_smart[5], po->vertices_cube_smart[6], po->normals_cube_smart[1]);
    cross(po->vertices_cube_smart[5], po->vertices_cube_smart[4], po->vertices_cube_smart[7], po->normals_cube_smart[2]);
    cross(po->vertices_cube_smart[4], po->vertices_cube_smart[0], po->vertices_cube_smart[3], po->normals_cube_smart[3]);
}

void make_cylinder(OBJECT *po, double size)
{

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
void draw_quad(GLfloat vertices[][4], GLfloat *normals, int iv1, int iv2, int iv3, int iv4, int ic) {
  glBegin(crt_render_mode); 
  {
    glColor3fv(colors[ic]);
    //glColor3f(1.0, 0.0, 0.0);
    glNormal3fv(normals);
    /*note the explicit use of homogeneous coords below: glVertex4f*/
    glVertex4fv(vertices[iv1]);
    glVertex4fv(vertices[iv2]);
    glVertex4fv(vertices[iv3]);
    glVertex4fv(vertices[iv4]);
  }
  glEnd();
}

/***********************************
 FUNCTION: draw_cube_smart() 
 ARGS: none
 RETURN: none
 DOES: draws a cube from quads
************************************/
void draw_cube_smart(OBJECT *po)
{
  /* sides */
  draw_quad(po->vertices_cube_smart, po->normals_cube_smart[0],0,1,2,3, BLUE);
  draw_quad(po->vertices_cube_smart, po->normals_cube_smart[1],1,5,6,2, BLUE);
  draw_quad(po->vertices_cube_smart, po->normals_cube_smart[2],5,4,7,6, BLUE);
  draw_quad(po->vertices_cube_smart, po->normals_cube_smart[3],4,0,3,7, BLUE);
    
  /* top and bottom */
  //draw_quad(po->vertices_cube_smart,3,2,6,7, BLUE);
  //draw_quad(po->vertices_cube_smart,0,1,5,4, BLUE);
}

/***********************************
 FUNCTION: draw_axes
 ARGS: none
 RETURN: none
 DOES: draws main X, Y, Z axes
************************************/
void draw_axes( void ) {
  glLineWidth( 5.0 );

 // glDisable(GL_LIGHTING);

  glBegin(GL_LINES); 
  {
    glColor3fv(colors[1]);
    glVertex4fv(vertices_axes[0]);
    glVertex4fv(vertices_axes[1]);
    
    glColor3fv(colors[4]);
    glVertex4fv(vertices_axes[0]);
    glVertex4fv(vertices_axes[2]);
    
    glColor3fv(colors[6]);
    glVertex4fv(vertices_axes[0]);
    glVertex4fv(vertices_axes[3]);
  }
  glEnd();
  glLineWidth( 1.0 );

 // glEnable(GL_LIGHTING);
  
}

void draw_objects() 
{
  crt_render_mode = GL_LINE_LOOP;
  int i;
  for(i=0; i<num_objects; i++)
  {
    printf("Iterating over std::deque<Shape> my_shapes! Accessing element %d in deque of size %d\n", i, (int)my_shapes.size());
    my_shapes[i].draw(crt_render_mode, false, colors[BLUE]); //for each shape in deque, draw it
  }
}

//Shape transformation functions:

void translateCamera(GLdouble x, GLdouble y, GLdouble z)
{
     printf("\nTranslating camera by {%g, %g, %g}\n", x, y, z);
     myCamera.translateCamera(x, y, z);
}

void translateCameraLocalized(GLdouble x, GLdouble y, GLdouble z)
{
    printf("\nTranslating LOCALIZED camera by {%g, %g, %g}\n", x, y, z);
    myCamera.translateCameraLocalized(x, y, z);
}

void rotateCameraLocalized(GLdouble deg, GLdouble x, GLdouble y, GLdouble z)
{
  printf("\nRotating LOCALIZED camera by {%g, %g, %g}\n", x, y, z);
  myCamera.rotateCameraLocalized(deg, x, y, z);
}

void cameraIncreaseHeight()
{
  myCamera.incrementHeight();
}

void cameraDecreaseHeight()
{
  myCamera.decrementHeight();
}

void decreaseNCP()
{
  myCamera.decreaseNCP();
}

void increaseNCP()
{
  myCamera.increaseNCP();
}

void increaseFCP()
{
  myCamera.increaseFCP();
}

void decreaseFCP()
{
  myCamera.decreaseFCP();
}

void cameraZoomIn()
{
  myCamera.zoomIn();
}

void cameraZoomOut()
{
  myCamera.zoomOut();
}

void aspectRatioIncrease()
{
  myCamera.increaseAR();
}

void aspectRatioDecrease()
{
  myCamera.decreaseAR();
}

void camPrint()
{
  myCamera.print();
}

void my_display() {

  // clear all pixels, reset depth 
  glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT );
  
  //glLoadIdentity();
  //setup the camera (1st person? 3rd person?)

  glLoadIdentity();
  myCamera.align(); 
  myCamera.transform(); 

  
  //draw the objects
  draw_axes();

  draw_objects();

  // this buffer is ready
  glutSwapBuffers();
}

void my_idle(void) {
  //EC idea: Make the flashlight flicker a bit (random flicker strength) when the user is idle.
  return ;
}
