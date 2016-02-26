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
#include "segment.h"
#include "paint.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - M A I N - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t main(int argc, char *argv[]){
  char **p = *&argv;
  FILE *OUTPUT = NULL, *INPUT = NULL;
  uint32_t n;
  
  PEYE = (EYEPARAM *) Malloc(1 * sizeof(EYEPARAM));
  if((PEYE->help = ArgsState(DEFAULT_HELP, p, argc, "-h")) == 1 || argc < 2){
    PrintMenuEye();
    return EXIT_SUCCESS;
    }

  if(ArgsState(DEF_VERSION, p, argc, "-V")){
    PrintVersion();
    return EXIT_SUCCESS;
    }

  PEYE->verbose   = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");
  PEYE->force     = ArgsState  (DEFAULT_FORCE,   p, argc, "-F");
  PEYE->width     = ArgsDouble (DEFAULT_WIDTH,   p, argc, "-w");
  PEYE->space     = ArgsDouble (DEFAULT_SPACE,   p, argc, "-a");
  PEYE->start     = ArgsDouble (0.35,            p, argc, "-s");
  PEYE->rotations = ArgsDouble (1.50,            p, argc, "-r");
  PEYE->hue       = ArgsDouble (1.92,            p, argc, "-u");
  PEYE->gamma     = ArgsDouble (0.50,            p, argc, "-g");
  PEYE->output    = ArgsFileGen(p, argc, "-o", "image", ".svg");

  if(!PEYE->force) 
    FAccessWPerm(PEYE->output);
  OUTPUT = Fopen(PEYE->output, "w");

  fprintf(stderr, "\n");
  if(PEYE->verbose){
    fprintf(stderr, "==[ CONFIGURATION ]=================\n");
    fprintf(stderr, "Verbose mode ....................... yes\n");
    fprintf(stderr, "Visual characteristics:\n");
    fprintf(stderr, "  [+] Width ........................ %.3g\n", PEYE->width);
    fprintf(stderr, "  [+] Space ........................ %.3g\n", PEYE->space);
    fprintf(stderr, "  [+] Start ........................ %.3g\n", PEYE->start);
    fprintf(stderr, "  [+] Rotations .................... %.3g\n", PEYE->rotations);
    fprintf(stderr, "  [+] Hue .......................... %.3g\n", PEYE->hue);
    fprintf(stderr, "  [+] Gamma ........................ %.3g\n", PEYE->gamma);
    fprintf(stderr, "Output visual filename ............. %s\n",   PEYE->output);
    fprintf(stderr, "\n");
    }

  fprintf(stderr, "==[ PROCESSING ]====================\n");
  TIME *Time = CreateClock(clock());

  int sym;
  char fname[MAX_NAME];
  double fvalue;
  uint64_t fsize;
 
  INPUT = Fopen(argv[argc-1], "r"); 
  while((sym = fgetc(INPUT)) != EOF){

    if(sym == '#'){
      if(fscanf(INPUT, "\t%lf\t%"PRIu64"\t%s\n", &fvalue, &fsize, fname) != 3){
        fprintf(stderr, "  [x] Error: unknown type of file!\n");
        exit(1);
        }
 
      // fprintf(stderr, "$\t%lf\t%"PRIu64"\t%s\n", fvalue, fsize, fname);
      fprintf(stderr, "  [+] Filtering %s\n", fname);
      
      
      // DO SOMETHING
      // RESET ?
      // MOVE PAINTER ?
      }

    // TODO: FILTER
    // TODO: SEGMENT
    }
  
  if(!OUTPUT) fclose(OUTPUT);
  if(!INPUT)  fclose(INPUT);

  StopTimeNDRM(Time, clock());
  fprintf(stderr, "Done!\n");
  fprintf(stderr, "\n");

  fprintf(stderr, "==[ RESULTS ]=======================\n");
  fprintf(stderr, "\n");

  fprintf(stderr, "==[ STATISTICS ]====================\n");
  StopCalcAll(Time, clock());
  fprintf(stderr, "\n");

  RemoveClock(Time);

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
