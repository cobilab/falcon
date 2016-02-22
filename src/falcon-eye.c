#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include "mem.h"
#include "time.h"
#include "defs.h"
#include "param.h"
#include "msg.h"
#include "common.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - M A I N - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t main(int argc, char *argv[]){
  char     **p = *&argv, **xargv, *xpl = NULL;
  int32_t  xargc = 0;
  uint32_t n, k, col, ref, topSize;
  double   gamma;
  Threads  *T;
  
  P = (Parameters *) Malloc(1 * sizeof(Parameters));
  if((P->help = ArgsState(DEFAULT_HELP, p, argc, "-h")) == 1 || argc < 2){
    PrintMenu();
    return EXIT_SUCCESS;
    }

  if(ArgsState(DEF_VERSION, p, argc, "-V")){
    PrintVersion();
    return EXIT_SUCCESS;
    }

  P->verbose  = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");
  P->force    = ArgsState  (DEFAULT_FORCE,   p, argc, "-F");
  P->sample   = ArgsNum    (DEFAULT_SAMPLE,  p, argc, "-p", MIN_SAP, MAX_SAP);
  P->level    = ArgsNum    (0,               p, argc, "-l", MIN_LEV, MAX_LEV);
  topSize     = ArgsNum    (DEF_TOP,         p, argc, "-t", MIN_TOP, MAX_TOP);
  P->output   = ArgsFileGen(p, argc, "-x", "top", ".csv");

  FILE *OUTPUT = NULL;
  if(!P->force) 
    FAccessWPerm(P->output);
  OUTPUT = Fopen(P->output, "w");

  fprintf(stderr, "\n");
  if(P->verbose) PrintArgs(P, T[0], argv[argc-2], argv[argc-1], topSize);

  fprintf(stderr, "==[ PROCESSING ]====================\n");
  TIME *Time = CreateClock(clock());
  P->base = argv[argc-1];
  fprintf(stderr, "Done!\n");

  StopTimeNDRM(Time, clock());
  fprintf(stderr, "\n");

  fprintf(stderr, "==[ RESULTS ]=======================\n");
  fprintf(stderr, "\n");

  fprintf(stderr, "==[ STATISTICS ]====================\n");
  StopCalcAll(Time, clock());
  fprintf(stderr, "\n");

  RemoveClock(Time);
  if(!OUTPUT) fclose(OUTPUT);

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
