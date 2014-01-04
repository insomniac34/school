/*****************************************************************
@Developers: Tyler Raborn & David Lange
@Course: CS/CoE 1541
@Assignment: Project 1 - Pipelined CPU Simulation
@Description: Header file for Pipeline class.
@Due: Oct. 9th, 2013 @ 11:59pm
@Development Environment: g++ 4.8.1, Windows 8 64-bit & Windows 7 32-bit
******************************************************************/

#include <deque>
#include <map>
#include "inttypes.h"
#include "trace_item.h"

#ifndef _PIPELINE_H
#define _PIPELINE_H

class Pipeline
{
	
private:

	//private fields
	int prediction_method;
	int trace_view_on; 
	int cycle_adjust;
	int mapSize;

	std::deque<struct trace_item> pipe; //STL queue; our main data structure containing instructions as they pass through the pipe.
	std::map<unsigned short, bool> predictionTable; //STL HashMap; used to implement branch prediction as per specsheet
	
	//private methods
	void instructionFetch();
	void instructionDecode();
	void execute();
	void dataMemory();
	void writeBack();
	void end();
	void begin();
	void print_branch(int, int);

public:

	//constructors
	Pipeline();
	explicit Pipeline(int, int, int);

	//public fields
	int insertionCount; //# of instructions inserted
	bool branch_taken_flag;

	//public methods
	void insert(struct trace_item*); //stick instruction in pipe
	void updatePipe();
	struct trace_item *getInstr(int);
	bool isFull();

	//destructor
	~Pipeline();

};

#endif
