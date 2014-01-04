Developers: Tyler P. Raborn & David J. Lange
Course: CS/CoE 1541
Assignment: Project 1 - Pipelined CPU Simulation
Due: Oct. 9th, 2013 @ 11:59pm
Development Environment: g++ 4.8.1, Windows 8 64-bit & Windows 7 32-bit

INCLUDED FILES:
Pipeline.h: Header file for our Pipeline class
Pipeline.cpp: Method definitions for our Pipeline class
main.cpp: The original Pipeline.c, modified to work with our Pipeline class
trace_item.h: original definition for trace_item struct

USAGE:
compilation:
g++ -o pipeline Pipeline.cpp main.cpp

execution:
./pipeline sample.tr <trace_view> <prediction method> <hash table size>

Description:
We used C++ to build a class called Pipeline, which essentially is a
wrapper class around a std::deque<> data structure that holds trace_item structs.
Using the Insert() method, instructions fetched from the original program are
inserted into the deque, operated on, and popped after having their information
displayed. The deque essentially acts as our pipeline as we push instructions through, 
perform the necessary operations, and pop them out.

To implement a branch prediction hash table, we used a std::map<unsigned short, bool> to map
bits 3-10 of the Addr field to the actual branch prediction.
