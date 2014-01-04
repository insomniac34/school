/*****************************************************************
@Developers: Tyler Raborn & David Lange
@Course: CS/CoE 1541
@Assignment: Project 1 - Pipelined CPU Simulation
@Description: Method declarations for Pipeline class. Simulates a simple 5-stage MIPS CPU pipeline.
@Due: Oct. 9th, 2013 @ 11:59pm
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

	begin();
}

void Pipeline::writeBack()
{
	//printf("Stage : . . . writeBack\n");
}

void Pipeline::dataMemory()
{

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

void Pipeline::begin()
{
	//printf("Stage : . . . Initial Buffer\n");

}

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
	if (insertionCount>0) printf("\n....................SIMULATION TERMINATED at cycle %d.........................\n\n", (insertionCount+cycle_adjust));
}
