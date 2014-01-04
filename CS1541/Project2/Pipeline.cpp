/*****************************************************************
@Developers: Tyler Raborn & David Lange
@Course: CS/CoE 1541
@Assignment: Project 2 - Pipelined CPU Simulation
@Description: Method declarations for Pipeline class. Simulates a simple 5-stage MIPS CPU pipeline.
@Due: Nov. 13th @ midnight
@Development Environment: g++ 4.8.1, Windows 8 64-bit & Windows 7 32-bit
******************************************************************/

#include <string>
#include <cstdlib>
#include <cstdio>
#include "Pipeline.h"

#define numStages 5

Pipeline::Pipeline() //def constructor to keep g++ happy
{

}

Pipeline::Pipeline(int trace_view_on, int prediction_method, int mapSize) //main constructor
{

	insertionCount = 0;
	cycle_adjust = 0;

	unsigned short index;
	for (index = 0; index < mapSize; index++) //initialize hashmap
	{
		predictionTable[index] = false;
	}

	this->mapSize = mapSize;
	this->trace_view_on = trace_view_on;
	this->prediction_method = prediction_method;
}

Pipeline::Pipeline(int trace_view_on, int prediction_method, int mapSize, int cacheSize, int blockSize, int assoc, int missPenalty)
{
	insertionCount = 0;
	cycle_adjust = 0;

    cacheHits = 0;
    cacheMisses = 0;
    cacheAccesses = 0;
    writeBacks = 0;

	unsigned short index;
	for (index = 0; index < mapSize; index++) //initialize hashmap
	{
		predictionTable[index] = false;
	}

	this->mapSize = mapSize;
	this->trace_view_on = trace_view_on;
	this->prediction_method = prediction_method;


	if ((cacheSize!=1 && cacheSize%2!=0) || (blockSize!=1 && blockSize%2!=0)) //check for powers of 2
	{
		printf("ERROR: Invalid Arguments; powers of two required.\n");
		exit(-1);
	}

	//Additional Project 2 Parameters
	this->totalCacheSize = cacheSize;
	this->blockSize = blockSize;
	this->assoc = assoc;
	this->missPenalty = missPenalty;

	//initialize cache:
	for (int i = 0; i < this->assoc; i++)
	{
		this->cacheList.push_front(Cache(cacheSize, blockSize, assoc));
	}

	this->numEntries = cacheList[0].getEntries();
	LRUarray = new int*[numEntries];
	for (int i = 0; i < numEntries; i++)
	{
		LRUarray[i] = new int[assoc];
		for (int j = 0; j < assoc; j++)
		{
			LRUarray[i][j] = 0; //initialize LRU tracker to 0
		}
	}

}

void Pipeline::insert(struct trace_item *curInstr)
{
	pipe.push_front(*curInstr); //insert current instruction into stage 1 of the pipeline
	insertionCount++;
	
	updatePipe();
}

void Pipeline::updatePipe() //moves each instruction currently in the pipe along; any instructions finished get placed into data structure for output
{
	while (pipe.size() > 5)
	{
		end(); //prints out info on the data that has exited the pipe
		this->pipe.pop_back(); //pop data at end of pipe; this deletes the data.
		//printf("Popping deque of size %d...\n", pipe.size());
	}

	//iterate backwards over CPU buffers:
	if (pipe.size() > 4) writeBack(); 
	if (pipe.size() > 3) dataMemory();
	if (pipe.size() > 2) execute();




	if (pipe.size() > 1) instructionDecode();
	if (pipe.size() > 0) instructionFetch();

	//begin();
}

void Pipeline::writeBack()
{
	//printf("Stage : . . . writeBack\n");
}

void Pipeline::dataMemory()
{
	//printf("Stage:: Data Memory\n");

	if (pipe[4].type == ti_LOAD || pipe[4].type == ti_STORE)
	{
        cacheAccesses++;

		std::string addrString;
		addrString.resize(32, '0');

		int curr_val = pipe[4].Addr;
		int i = 0;
		
		do //convert hex address to binary
		{
			int r = curr_val%2;
			int q = curr_val / 2;
			if (r==1)
			{
				addrString[i] = '1';
			}
			i++;
			curr_val = q;

			//printf("q: %d\n", q);
			//printf("r: %d\n", r);

		} while(curr_val != 0);

		// printf("Addr = %X\n", pipe[4].Addr);
		//binary string loaded
		// printf("addrString: %s\n", addrString.c_str());

		//printf("checking bit string\n");
		int byte_offset = 2;
		int block_bits = log2(blockSize);
		int index_bits = log2(cacheList[0].numEntries);
		int tag_bits = (32 - (byte_offset + block_bits + index_bits));
		int index_val = 0;
		int tag_val = 0;
		int ex = 0;
		int low_bits = byte_offset + block_bits + index_bits;

		//printf("block_bits = %d\nindex_bits = %d\ntag_bits = %d\n", block_bits, index_bits, tag_bits);

		for (i = byte_offset + block_bits; i < (block_bits + byte_offset + index_bits); i++)
		{
			int curr_bit = 0;
			if (addrString[i] == '1')
			{
				curr_bit = 1;
			}
			index_val += curr_bit*pow(2, ex);
			ex++;
		}

		//printf("between addr calculations\n");

		ex = 0;
		for (i = low_bits; i < 32; i++)
		{
			int curr_bit = 0;
			if (addrString[i] == '1')
			{
				curr_bit = 1;
			}
			
			tag_val += curr_bit*pow(2, ex);
			ex++;
		}


		//printf("accessing cache\n");
		bool cache_hit = false;
		for (i = 0; i < cacheList.size(); i++)
		{
			if (cacheList[i].blockList[index_val].valid == 1 && tag_val == cacheList[i].blockList[index_val].tag)
			{
				cache_hit = true;

                if (pipe[4].type == ti_STORE)
                {
                    cacheList[i].blockList[index_val].dirty = 1;
                }
				// updating LRU
				for (int j = 0; j < assoc; j++)
				{
					// set this block to be the MRU
					if (j == i)
					{
						//printf("CACHE HIT! At index %d, current cache tag: %d, inserting tag: %d\n", index_val, cacheList[j].blockList[index_val].tag, tag_val);
						LRUarray[index_val][j] = 1;
					}
					// update the other entries
					else
					{
						if (LRUarray[index_val][j] != 0)
						{
							LRUarray[index_val][j] += 1;
						}
						
					}
				}

                break;
			}
		}
		
		//check results of query:
		if (cache_hit==false)
		{
            cacheMisses++;
			/////////////////ADJUST GLOBAL PENALTY///////////////////////////////////////////
			cycle_adjust+=missPenalty;
			/////////////////////////////////////////////////////////////////////////////////

			int lru_index = -1;
			int max_priority = -1;
			for (int j = 0; j < assoc; j++)
			{
				if (LRUarray[index_val][j] != 0 && LRUarray[index_val][j] > max_priority)
				{
					max_priority = LRUarray[index_val][j];
					lru_index = j;
					//break;
				}
				else if (LRUarray[index_val][j] == 0)
				{
					lru_index = j;
					break;
				}
			}
			for (int j = 0; j < assoc; j++)
			{
				if (j == lru_index)
				{
					//printf("cache miss! At index %d, current cache tag: %d, inserting tag: %d\n", index_val, cacheList[j].blockList[index_val].tag, tag_val);
					LRUarray[index_val][j] = 1;
                    if (cacheList[j].blockList[index_val].dirty == 1)
                    {
                        writeBacks++;
                    }
					cacheList[j].blockList[index_val].valid = 1;
					cacheList[j].blockList[index_val].tag = tag_val;
                    cacheList[j].blockList[index_val].dirty = 0;
				}
				else
				{
					if (LRUarray[index_val][j] != 0)
					{
						LRUarray[index_val][j] += 1;
					}
				}
			}
		}
        else
        {
            cacheHits++;
        }

	}
}

void Pipeline::execute()
{
	//printf("Stage : . . . execute\n");
	if (pipe[3].type == ti_BRANCH)
	{
		if (prediction_method == 0)
		{
			if (pipe[3].Addr == pipe[2].PC)
			{			
				// printf("\nBRANCH TAKEN IN EXECUTE!\n");
				this->print_branch(4, 1);
				this->print_branch(3, 2);
				//If the address in index [2] is = to the PC at instruction [0], SQUASH the instruction at index [1]
				pipe[3].type = ti_NOP;
				pipe[4].type = ti_NOP;
				cycle_adjust += 2;
			}
		}
		else if (prediction_method == 1)
		{
			if (pipe[3].Addr == pipe[2].PC)
			{
				struct trace_item NOOP;
				
				NOOP.type = ti_NOP;
				NOOP.PC = pipe[3].PC + 4;
				print_branch(6, 1);
				pipe.insert(pipe.begin()+3, NOOP);
				cycle_adjust += 1;
				
				this->pipe.pop_back();
				
			}
			else if (pipe[3].PC + 4 == pipe[2].PC)
			{
				struct trace_item NOOP;
				
				NOOP.type = ti_NOP;
				NOOP.PC = pipe[3].Addr;
				print_branch(6, 1);
				pipe.insert(pipe.begin()+2, NOOP);
				cycle_adjust += 1;
				
				this->pipe.pop_back();
				
			}
		}
		else if (prediction_method == 2) //1-bit branch predictor utilized
		{	
			unsigned short keyBits = (pipe[3].Addr & 4088) >> 3; // grab high order 7 bits of addr; AND'd with 31 to grab 3-7, right shifted by 3 to acquire index
			if (predictionTable[keyBits%mapSize] == false) // if predicted not taken
			{
				if (pipe[2].PC == pipe[3].PC+4) // if next instr. == PC + 4: do nothing
				{
					predictionTable[keyBits%mapSize] = false;
				}
				else if (pipe[2].PC == pipe[3].Addr) // if next instr. == Addr: 2 no-ops after branch instr.
				{
					//printf("PREDICTION INCORRECT! Predicted not taken, branch taken\n");
					this->print_branch(4, 1);
					this->print_branch(3, 2);
					pipe[3].type = ti_NOP;
					pipe[4].type = ti_NOP;
					cycle_adjust += 2;

					predictionTable[keyBits%mapSize] = true;
				}
			}
			else if (predictionTable[keyBits%mapSize] == true) // if predicted taken
			{
				if (pipe[2].PC == pipe[3].PC+4)// if next instr. == PC + 4: 2 no-ops after branch instr.
				{
					//printf("PREDICTION INCORRECT! Predicted taken, branch not taken\n");
					this->print_branch(4, 1);
					this->print_branch(3, 2);
					pipe[3].type = ti_NOP;
					pipe[4].type = ti_NOP;
					cycle_adjust += 2;

					predictionTable[keyBits%mapSize] = false;
				}
				else if(pipe[2].PC == pipe[3].Addr) // if next instr. == Addr: do nothing
				{
					predictionTable[keyBits%mapSize] = true;
				}
			}
		}
	}

	if (pipe[3].type == ti_LOAD) //if cur instr in EX is a Load
	{
		if ((pipe[3].sReg_a == pipe[2].sReg_b) || (pipe[3].sReg_a == pipe[2].dReg)) //if the instr in EX and the instr in ID have a data dependency...
		{
			//printf("STALLING PIPE; Load Word instruction detected!\n");
			this->print_branch(4, 1);
			this->print_branch(3, 2);
			pipe[3].type = ti_NOP;
			pipe[4].type = ti_NOP;
			cycle_adjust += 2;
		}
	}

	//if jump...clear instruction by making it a noop by inserting into stage 2
	if (pipe[3].type == ti_JTYPE)
	{
		//printf("JUMP!\n");
		struct trace_item NOOP;
		NOOP.type = ti_NOP;
		NOOP.PC = pipe[3].Addr+4;
		
		pipe.insert(pipe.begin()+3, NOOP);
		print_branch(pipe.size()-1, 1);
		pipe.pop_back();
		//NOOP.type = ti_NOP;
		//NOOP.PC = pipe[3].PC + 4;
		cycle_adjust += 1;
	}
}

void Pipeline::instructionDecode()
{
	//printf("Stage : . . . decode/regs\n");
}

void Pipeline::instructionFetch()
{
	//printf("Stage : . . . instructionFetch\n");
}

void Pipeline::end() //prints out details on the instruction having just exited the buffer
{
	if (this->isFull() == true) 
	{
		//TODO: switch around the reg values depending on instruction type.

		std::string instructionType;
		std::string reg2 = "sReg_b";
		std::string reg3 = "addr";

		unsigned int programCounter = this->pipe[5].PC;
		unsigned int memAddress = this->pipe[5].Addr;

		if (trace_view_on == 1)
		{
			switch(this->pipe[5].type)
			{
				case ti_RTYPE:
					instructionType = "R-type";
					reg3 = "dReg";
					printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %d)\n", insertionCount+cycle_adjust, instructionType.c_str(), programCounter, this->pipe[5].sReg_a, reg2.c_str(), this->pipe[5].sReg_b, reg3.c_str(), this->pipe[5].dReg);
					break;

				case ti_ITYPE:
					instructionType = "I-type";
					reg2 = "dReg";
					printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", insertionCount+cycle_adjust, instructionType.c_str(), programCounter, this->pipe[5].sReg_a, reg2.c_str(), this->pipe[5].dReg, reg3.c_str(), memAddress);
					break;

				case ti_LOAD:
					instructionType = "Load";
					reg2 = "dReg";
					printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", insertionCount+cycle_adjust, instructionType.c_str(), programCounter, this->pipe[5].sReg_a, reg2.c_str(), this->pipe[5].dReg, reg3.c_str(), memAddress);
					break;

				case ti_STORE:
					instructionType = "Store";
					printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", insertionCount+cycle_adjust, instructionType.c_str(), programCounter, this->pipe[5].sReg_a, reg2.c_str(), this->pipe[5].sReg_b, reg3.c_str(), memAddress);
					break;

				case ti_BRANCH:
					instructionType = "Branch";
					printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", insertionCount+cycle_adjust, instructionType.c_str(), programCounter, this->pipe[5].sReg_a, reg2.c_str(), this->pipe[5].sReg_b, reg3.c_str(), memAddress);
					break;

				case ti_JTYPE:
					instructionType = "J-type";
					printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", insertionCount+cycle_adjust, instructionType.c_str(), programCounter, this->pipe[5].sReg_a, reg2.c_str(), this->pipe[5].sReg_b, reg3.c_str(), memAddress);
					break;

				case ti_SPECIAL:
					instructionType = "Special";
					printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", insertionCount+cycle_adjust, instructionType.c_str(), programCounter, this->pipe[5].sReg_a, reg2.c_str(), this->pipe[5].sReg_b, reg3.c_str(), memAddress);
					break;

				case ti_JRTYPE:
					instructionType = "JR-type";
					printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", insertionCount+cycle_adjust, instructionType.c_str(), programCounter, this->pipe[5].sReg_a, reg2.c_str(), this->pipe[5].sReg_b, reg3.c_str(), memAddress);
					break;

				case ti_NOP:
					instructionType = "NO-OP";
					printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x)\n", insertionCount+cycle_adjust, instructionType.c_str(), programCounter);
					break;
			}
		}
	}
}

/*
void Pipeline::begin()
{
	//printf("Stage : . . . Initial Buffer\n");

}*/

bool Pipeline::isFull()
{
	if (insertionCount >= 5) return true;	
	return false;	
}

void Pipeline::print_branch(int index, int instr_x)
{
	std::string instructionType;
	std::string reg2 = "sReg_b";
	std::string reg3 = "addr";

	unsigned int programCounter = this->pipe[index].PC;
	unsigned int memAddress = this->pipe[index].Addr;
	int ins_count = insertionCount + instr_x + cycle_adjust;
	
	if (trace_view_on == 1)
	{
		switch(this->pipe[index].type)
		{
			case ti_RTYPE:
				instructionType = "R-type";
				reg3 = "dReg";
				printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", ins_count, instructionType.c_str(), programCounter, this->pipe[index].sReg_a, reg2.c_str(), this->pipe[index].sReg_b, reg3.c_str(), this->pipe[index].dReg);
				break;

			case ti_ITYPE:
				instructionType = "I-type";
				reg2 = "dReg";
				printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", ins_count, instructionType.c_str(), programCounter, this->pipe[index].sReg_a, reg2.c_str(), this->pipe[index].dReg, reg3.c_str(), memAddress);
				break;

			case ti_LOAD:
				instructionType = "Load";
				reg2 = "dReg";
				printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", ins_count, instructionType.c_str(), programCounter, this->pipe[index].sReg_a, reg2.c_str(), this->pipe[index].dReg, reg3.c_str(), memAddress);
				break;

			case ti_STORE:
				instructionType = "Store";
				printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", ins_count, instructionType.c_str(), programCounter, this->pipe[index].sReg_a, reg2.c_str(), this->pipe[index].sReg_b, reg3.c_str(), memAddress);
				break;

			case ti_BRANCH:
				instructionType = "Branch";
				printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", ins_count, instructionType.c_str(), programCounter, this->pipe[index].sReg_a, reg2.c_str(), this->pipe[index].sReg_b, reg3.c_str(), memAddress);
				break;

			case ti_JTYPE:
				instructionType = "J-type";
				printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", ins_count, instructionType.c_str(), programCounter, this->pipe[index].sReg_a, reg2.c_str(), this->pipe[index].sReg_b, reg3.c_str(), memAddress);
				break;

			case ti_SPECIAL:
				instructionType = "Special";
				printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", ins_count, instructionType.c_str(), programCounter, this->pipe[index].sReg_a, reg2.c_str(), this->pipe[index].sReg_b, reg3.c_str(), memAddress);
				break;

			case ti_JRTYPE:
				instructionType = "JR-type";
				printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x) (sReg_a: %d) (%s: %d) (%s: %x)\n", ins_count, instructionType.c_str(), programCounter, this->pipe[index].sReg_a, reg2.c_str(), this->pipe[index].sReg_b, reg3.c_str(), memAddress);
				break;

			case ti_NOP:
				instructionType = "NO-OP";
				printf("\nCurrent instruction exiting pipe : . . . [Cycle: %d] [Format: %s] (PC: %x)\n", ins_count, instructionType.c_str(), programCounter);
				break;
		}
	}
}

Pipeline::~Pipeline() //default destructor
{
	//exit message:
	if (insertionCount>0)
    {
        printf("\n....................SIMULATION TERMINATED at cycle %d .........................\n\n", (insertionCount+cycle_adjust));
        printf("cache accesses: %d\n", cacheAccesses);
        printf("cache hits: %d\n", cacheHits);
        printf("cache misses: %d\n", cacheMisses);
        printf("write backs: %d\n", writeBacks);
    }
	
	/*for (int i = 0; i < numEntries; i++)
	{
		printf("Entry %d: ", i);
		for (int j = 0; j < assoc; j++)
		{
			printf("\t%d\n", cacheList[j].blockList[i].tag);
		}

	}*/

	for (int i = 0; i < numEntries; i++)
	{	
		delete[] LRUarray[i];
	}
	delete[] LRUarray;

}
