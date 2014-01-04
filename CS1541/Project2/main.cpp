/*****************************************************************
@Developers: Tyler Raborn & David Lange
@Course: CS/CoE 1541
@Assignment: Project 2 - Pipelined CPU Simulation
@Due: Nov. 13th @ midnight
@Development Environment: g++ 4.8.1, Windows 8 64-bit & Windows 7 32-bit
******************************************************************/

#include <cstdio>
#include <cstdlib>
#include <inttypes.h>
#include <arpa/inet.h>
#include "trace_item.h" 
#include "Pipeline.h"

#define TRACE_BUFSIZE 1024*1024

static FILE *trace_fd;
static int trace_buf_ptr;
static int trace_buf_end;
static struct trace_item *trace_buf;

Pipeline *myPipeline; 

int isBigEndian(void) 
{
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1; 
}

uint32_t littleEndianConverter(uint32_t x) //Based on ntohl(); the ntohl() function converts the unsigned 32-bit integer x from network byte order to host byte order
{
  u_char *s = (u_char *)&x;
  return (uint32_t)(s[3] << 24 | s[2] << 16 | s[1] << 8 | s[0]);
}

void initialize() //instantiates trace_buf
{
  trace_buf = (struct trace_item*)malloc(sizeof(struct trace_item) * TRACE_BUFSIZE);

  if (!trace_buf) {
    fprintf(stdout, "** trace_buf not allocated\n");
    exit(-1);
  }

  trace_buf_ptr = 0;
  trace_buf_end = 0;
}

void deallocate() //free memory & close streams
{
  free(trace_buf);
  fclose(trace_fd);
  //printf("DELETING!\n");
  delete myPipeline;
}

int getInstruction(struct trace_item **item) //this function points the passed-in instruction struct to the NEXT instruction.
{
  int n_items;

  if (trace_buf_ptr == trace_buf_end) {	/* if no more unprocessed items in the trace buffer, get new data  */
    n_items = fread(trace_buf, sizeof(struct trace_item), TRACE_BUFSIZE, trace_fd);
    if (!n_items) return 0;				/* if no more items in the file, we are done */

    trace_buf_ptr = 0;
    trace_buf_end = n_items;			/* n_items were read and placed in trace buffer */
  }

  *item = &trace_buf[trace_buf_ptr];	/* read a new trace item for processing */
  trace_buf_ptr++;
  
  if (isBigEndian()) {
    (*item)->PC = littleEndianConverter((*item)->PC);
    (*item)->Addr = littleEndianConverter((*item)->Addr);
  }

  return 1;
}

int main(int argc, char **argv)
{

  if (argc == 5)myPipeline = new Pipeline(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
  else if (argc==9) myPipeline = new Pipeline(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8]));
  else myPipeline = new Pipeline(0, 0, 128);

  struct trace_item *tr_entry;
  size_t size;
  char *trace_file_name;
  int trace_view_on = 0;
  
  unsigned char t_type = 0;
  unsigned char t_sReg_a= 0;
  unsigned char t_sReg_b= 0;
  unsigned char t_dReg= 0;
  unsigned int t_PC = 0;
  unsigned int t_Addr = 0;

  unsigned int cycle_number = 0;

  if (argc == 1) {
    fprintf(stdout, "\nUSAGE: tv <trace_file> <switch - any character>\n");
    fprintf(stdout, "\n(switch) to turn on or off individual item view.\n\n");
    exit(0);
  }
    
  trace_file_name = argv[1];
  if (argc == 3) trace_view_on = atoi(argv[2]) ;

  fprintf(stdout, "\n ** opening file %s\n", trace_file_name);

  trace_fd = fopen(trace_file_name, "rb");

  if (!trace_fd) {
    fprintf(stdout, "\ntrace file %s not opened.\n\n", trace_file_name);
    exit(0);
  }

  initialize();

  while(1) 
  {
    size = getInstruction(&tr_entry);
   
    if (!size) //if size is empty, no more instructions...
    {       /* no more instructions (trace_items) to simulate */
     // printf("\n....................SIMULATION TERMINATED at cycle %d.........................\n\n", myPipeline.insertionCount);
      break;
    }
    else
    {              /* parse the next instruction to simulate */
         cycle_number++;
     
         myPipeline->insert(tr_entry);

      /*t_type = tr_entry->type;
      t_sReg_a = tr_entry->sReg_a;
      t_sReg_b = tr_entry->sReg_b;
      t_dReg = tr_entry->dReg;
      t_PC = tr_entry->PC;
      t_Addr = tr_entry->Addr;*/
    }  

    // SIMULATION OF A SINGLE CYCLE CPU IS TRIVIAL - EACH INSTRUCTION IS EXECUTED
    // IN ONE CYCLE

    /*if (trace_view_on) 
    {
      switch(tr_entry->type) 
      {
        case ti_NOP:
          printf("[cycle %d] NOP:",cycle_number) ;
          break;
        case ti_RTYPE:
          printf("[cycle %d] RTYPE:",cycle_number) ;
          printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(dReg: %d) \n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->dReg);
          break;
        case ti_ITYPE:
          printf("[cycle %d] ITYPE:",cycle_number) ;
          printf(" (PC: %x)(sReg_a: %d)(dReg: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->dReg, tr_entry->Addr);
          break;
        case ti_LOAD:
          printf("[cycle %d] LOAD:",cycle_number) ;      
          printf(" (PC: %x)(sReg_a: %d)(dReg: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->dReg, tr_entry->Addr);
          break;
        case ti_STORE:
          printf("[cycle %d] STORE:",cycle_number) ;      
          printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->Addr);
          break;
        case ti_BRANCH:
          printf("[cycle %d] BRANCH:",cycle_number) ;
          printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->Addr);
          break;
        case ti_JTYPE:
          printf("[cycle %d] JTYPE:",cycle_number) ;
          printf(" (PC: %x)(addr: %x)\n", tr_entry->PC,tr_entry->Addr);
          break;
        case ti_SPECIAL:
          printf("[cycle %d] SPECIAL:",cycle_number) ;      	
          break;
        case ti_JRTYPE:
          printf("[cycle %d] JRTYPE:",cycle_number) ;
          printf(" (PC: %x) (sReg_a: %d)(addr: %x)\n", tr_entry->PC, tr_entry->dReg, tr_entry->Addr);
          break;
      }
    }*/
  }

  deallocate();

  exit(0);
}

