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

  PEYE->verbose    = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");
  PEYE->force      = ArgsState  (DEFAULT_FORCE,   p, argc, "-F");
  PEYE->width      = ArgsDouble (DEFAULT_WIDTH,   p, argc, "-w");
  PEYE->space      = ArgsDouble (DEFAULT_SPACE,   p, argc, "-s");
  PEYE->start      = ArgsDouble (0.35,            p, argc, "-i");
  PEYE->rotations  = ArgsDouble (1.50,            p, argc, "-r");
  PEYE->hue        = ArgsDouble (1.92,            p, argc, "-u");
  PEYE->gamma      = ArgsDouble (0.50,            p, argc, "-g");
  PEYE->output     = ArgsFileGen(p, argc, "-o", "femap", ".svg");

  if(!PEYE->force) 
    FAccessWPerm(PEYE->output);
  OUTPUT = Fopen(PEYE->output, "w");

  fprintf(stderr, "\n");
  if(PEYE->verbose){
    PrintArgsEye(PEYE);
    }

  fprintf(stderr, "==[ PROCESSING ]====================\n");
  TIME *Time = CreateClock(clock());

  int sym;
  char fname[MAX_NAME];
  double fvalue;
  uint64_t fsize, iPos, ePos;

  INPUT = Fopen(argv[argc-1], "r"); 
  while((sym = fgetc(INPUT)) != EOF){

    if(sym == '$'){
      if(fscanf(INPUT, "\t%lf\t%"PRIu64"\t%s\n", &fvalue, &fsize, fname) != 3){
        fprintf(stderr, "  [x] Error: unknown type of file!\n");
        exit(1);
        }

      // TODO: Paint global map
      fprintf(OUTPUT, "&%.3lf\n", fvalue);
 
      fprintf(stderr, "  [+] Painting %s ... ", fname);
      while(1){
        off_t beg = Ftello(INPUT);
        if(fscanf(INPUT, "%"PRIu64":%"PRIu64"\n", &iPos, &ePos) != 2){
          Fseeko(INPUT, (off_t) beg, SEEK_SET);
          break;
          }
        else{
          // TODO: Paint local map
          fprintf(OUTPUT, "%"PRIu64":%"PRIu64"\n", iPos, ePos);
          }
        }

      fprintf(stderr, "Done!\n");
      }
    }

  // TODO: PaintFinal();
  
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
