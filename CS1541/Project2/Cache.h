/*****************************************************************
@Developers: Tyler Raborn & David Lange
@Course: CS/CoE 1541
@Assignment: Project 2 - Pipelined CPU Simulation
@Description: Header file for our Cache class
@Due: Nov. 13th @ midnight
@Development Environment: g++ 4.8.1, Windows 8 64-bit & Windows 7 32-bit
******************************************************************/

#include <cstdio>
#include <cmath>
#include <deque>
#include "trace_item.h"

#ifndef _CACHE_H
#define _CACHE_H

class Cache //a "wrapper class" to represent a cache
{

public:

	struct block //represents each entry in the cache.
	{
		unsigned int valid;
		unsigned int tag;
        unsigned int dirty;

	};	

	std::deque<block> blockList; //represents a cache of n blocks.

	int numEntries;

	Cache();
	explicit Cache(int, int, int);
	~Cache();

	int getEntries();
};

#endif