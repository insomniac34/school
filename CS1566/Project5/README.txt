Tyler Raborn
CS1566 Project 6 - README.txt
DEV ENVIRONMENT: Ubuntu Linux 13.10 64-bit
COMPILATION COMMAND: g++ -o test0 Camera.cpp Face.cpp glmain.cpp matrix.cpp Shape.cpp Vertex.cpp -lglut -lGL -lGLU


I got sphere intersections to work. Cone is movable, shoots a ray from its local x-axis. Is rotatable/translatable.

CONTROLS:
a/w/s/d allows you to move about the world FPS style.
THe ARROW keys allow you to move the cone about, and holding down 'c' while using the arrow keys allows you to ROTATE the clinder, and thereby
its local axis from which it shoots. 


EXTRA CREDIT:
If it's possible to get anything, I added jumping by hitting the spacebar, and going up to the cone and pressing C allows you to "pick up" the cone and rotate it as you stand.
You cannot translate this way, however. 

