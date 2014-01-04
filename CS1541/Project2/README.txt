Tyler Raborn & David Lange
CS1541 Project 2 README.txt
DUE: 11/13/2013 @ midnight
DEVELOPMENT ENVIRONMENT: Windows 8 64-bit & Windows 7 32-bit, g++ compiler via Cygwin
COMPILATION COMMAND: g++ -o test main.cpp Pipeline.cpp Cache.cpp
RUN COMMAND: ./test0 <trace_file> <trace_view_on> <prediction_method> <map_size> <cache_size> <block_size> <assoc> <penalty>


Included Files:
trace_item.h - an UNMODIFIED header file for the instruction structure definition
main.cpp - a modified version of Dr. Melhem's original instruction parsing C program.
Pipeline.cpp - method definitions for our Pipeline class that simulates the CPU & its operations
Pipeline.h - pipeline class header file
Cache.cpp - method definitions for our class that simulates a simple n-way set-associative cache
cache.h - our cache class header file
Experiment_Results.xlsx - our tabulation of each experiment's results as well as the conclusion drawn from each.
command_line.txt - the command line input/output from each experiment, as requested in the Project Specification
README.txt

Comments: Dr. Melhem gave his OK for us to use our C++ source code instead of just a single .C file.
The Pipeline class manages the core functionality of the simulator, while the Cache class
handles the behavior of an arbitrarily sized cache. To represent N-way set associativity in our CPU
we used a queue of Cache objects.


Bugs:
The program appears to compile & run without error.