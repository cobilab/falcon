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
#include "filters.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - M A I N - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t main(int argc, char *argv[]){
  char **p = *&argv;
  FILE *OUTPUT = NULL, *INPUT = NULL;
  
  PEYE = (EYEPARAM *) Malloc(1 * sizeof(EYEPARAM));
  if((PEYE->help = ArgsState(DEFAULT_HELP, p, argc, "-h")) == 1 || argc < 2){
    PrintMenuFilter();
    return EXIT_SUCCESS;
    }

  if(ArgsState(DEF_VERSION, p, argc, "-V")){
    PrintVersion();
    return EXIT_SUCCESS;
    }

  PEYE->verbose    = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");
  PEYE->force      = ArgsState  (DEFAULT_FORCE,   p, argc, "-F");
  PEYE->windowSize = ArgsNum    (100,             p, argc, "-s", 1, 999999);
  PEYE->windowType = ArgsNum    (1,               p, argc, "-w", 0, 3);
  PEYE->sampling   = ArgsNum    (10,              p, argc, "-x", 1, 999999);
  PEYE->threshold  = ArgsDouble (1.0,             p, argc, "-t");
  PEYE->lowerSimi  = ArgsDouble (0.00,            p, argc, "-sl");
  PEYE->upperSimi  = ArgsDouble (100.00,          p, argc, "-su");
  PEYE->lowerSize  = ArgsNum64  (1,               p, argc, "-dl", 1,
  9999999999);
  PEYE->upperSize  = ArgsNum64  (9999999999,      p, argc, "-du", 1,
  9999999999);
  PEYE->output     = ArgsFileGen(p, argc, "-o", "coords", ".fil");

  if(!PEYE->force) 
    FAccessWPerm(PEYE->output);
  OUTPUT = Fopen(PEYE->output, "w");

  fprintf(stderr, "\n");
  if(PEYE->verbose){
    PrintArgsFilter(PEYE);
    }

  fprintf(stderr, "==[ PROCESSING ]====================\n");
  TIME *Time = CreateClock(clock());

  int sym;
  char fname[MAX_NAME];
  double fvalue;
  uint64_t fsize;

  FILTER *FIL = CreateFilter(PEYE->windowSize, PEYE->sampling, PEYE->windowType, 
  PEYE->threshold);
  INPUT = Fopen(argv[argc-1], "r"); 
  while((sym = fgetc(INPUT)) != EOF){
    if(sym == '#'){
      if(fscanf(INPUT, "\t%lf\t%"PRIu64"\t%s\n", &fvalue, &fsize, fname) != 3){
        fprintf(stderr, "  [x] Error: unknown type of file!\n");
        exit(1);
        }

      if(fsize > PEYE->upperSize ||  fsize < PEYE->lowerSize ||
        fvalue > PEYE->upperSimi || fvalue < PEYE->lowerSimi)
        continue;

      fprintf(stderr, "  [+] Filtering & segmenting %s ... ", fname);
      fprintf(OUTPUT, "$\t%lf\t%"PRIu64"\t%s\n", fvalue, fsize, fname);
      InitEntries(FIL, fsize, INPUT);
      FilterStream(FIL, OUTPUT);
      DeleteEntries(FIL);
      fprintf(stderr, "Done!\n");
      }
    }
  DeleteFilter(FIL);
  
  if(!OUTPUT) fclose(OUTPUT);
  if(!INPUT)  fclose(INPUT);

  StopTimeNDRM(Time, clock());
  fprintf(stderr, "\n");

  fprintf(stderr, "==[ STATISTICS ]====================\n");
  StopCalcAll(Time, clock());
  fprintf(stderr, "\n");

  RemoveClock(Time);

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
