Tyler Raborn
CS1566 Project 4 README
Development Environment: Xubuntu Linux 64-bit, g++ compiler

Included Files:
Shape.h
Shape.cpp
Face.h
Face.cpp
Vertex.h
Vertex.cpp
Camera.h
Camera.cpp
matrix.h
matrix.cpp
glmain.h
glmain.cpp

COMPILE INSTRUCTIONS: g++ -o myCam Face.cpp glmain.cpp matrix.cpp Shape.cpp Vertex.cpp Camera.cpp -lglut -lGLU -lGL

BUGS/INCOMPLETE:
Not really any bugs as far as I'm aware. I didn't include a print function, as I ran out of time.

comments:
I didn't use the built-in infrastructure for shape management, I used C++ deques for that instead
inside of glmain. Made a Camera class.