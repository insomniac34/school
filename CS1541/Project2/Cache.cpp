/*****************************************************************
@Developers: Tyler Raborn & David Lange
@Course: CS/CoE 1541
@Assignment: Project 2 - Pipelined CPU Simulation
@Description: Method declarations for Cache class. Simulates a simple cache.
@Due: Nov. 13th @ midnight
@Development Environment: g++ 4.8.1, Windows 8 64-bit & Windows 7 32-bit
******************************************************************/

#include "Cache.h"

Cache::Cache()
{

}

Cache::Cache(int cacheSize, int blockSize, int assoc)
{
	float sizeKB = (float)cacheSize / (float)assoc; //kb
	int numWords = (sizeKB * pow(2, 10)) / 4; //convert to words
	numEntries = (float)numWords / (float)blockSize;  //number of entries 

	//printf("Initializing cache object; cacheSize = %d, blockSize = %d, assoc = %d, sizeKB = %d, numWords = %d, numEntries = %d\n", cacheSize, blockSize, assoc, sizeKB, numWords, numEntries);

	for(int i = 0; i < numEntries; i++)
	{
		block curBlock;
		curBlock.valid = 0;
		curBlock.tag = 0;

		blockList.push_front(curBlock);
	}

}

int Cache::getEntries()
{
	return this->numEntries;
}

Cache::~Cache()
{

}