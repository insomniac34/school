
/* @Programmer: Modified by Tyler Raborn, originally gem, based on hwa's
 * @Assignment: CS1566 Assignment 1
 * @Environment: Visual Studio 2012, Windows 8 Pro 64-bit, classic GLUT library installed directly into Visual Studio's Visual C/C++ lib/include directories.
 * @Description: This .h file provides the outline of the main hellomouse program.
 */

#ifndef _HELLOMOUSE_H
#define _HELLOMOUSE_H

void glut_setup(void) ;
void gl_setup(void) ;
void my_setup(void);
void my_display(void) ;
void my_reshape(int w, int h) ;
void my_idle(void) ;
void my_mouse(int b, int s, int x, int y) ;
void my_keyboard(unsigned char c, int x, int y) ;
void my_TimeOut(int id) ;

#endif
