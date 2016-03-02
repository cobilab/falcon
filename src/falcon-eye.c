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
  size_t len;
  char fname[MAX_NAME], *line = NULL;
  double fvalue;
  uint64_t maxSize, fsize, iPos, ePos, nSeq;
  Painter *Paint;

  // OPTION TO IGNORE SCALE
  //TODO: NEED TO GET MAXIMUM TO SET SCALE
  //TODO: NEED TO GET NUMBER OF SEQUENCES

  INPUT = Fopen(argv[argc-1], "r"); 

  nSeq = 0;
  while((sym = fgetc(INPUT)) != EOF){
    if(sym == '$'){
      if(fscanf(INPUT, "\t%lf\t%"PRIu64"\t%s\n", &fvalue, &fsize, fname) != 3){
        fprintf(stderr, "  [x] Error: unknown type of file!\n");
        exit(1);
        }
      if(fsize > maxSize)
        maxSize = fsize;
      ++nSeq;
      }
    }
  rewind(INPUT);

  SetScale(maxSize);
  Paint = CreatePainter(GetPoint(maxSize), "#ffffff");

  PrintHead(OUTPUT, (2 * DEFAULT_CX) + (((Paint->width + DEFAULT_SPACE) *
  nSeq) - DEFAULT_SPACE), Paint->size + EXTRA);
  Rect(OUTPUT, (2 * DEFAULT_CX) + (((Paint->width + DEFAULT_SPACE) *
  nSeq) - DEFAULT_SPACE), Paint->size + EXTRA, 0, 0, "#ffffff");

  while((sym = fgetc(INPUT)) != EOF){

    if(sym == '$'){
      if(fscanf(INPUT, "\t%lf\t%"PRIu64"\t%s\n", &fvalue, &fsize, fname) != 3){
        fprintf(stderr, "  [x] Error: unknown type of file!\n");
        exit(1);
        }

      // TODO: Paint global map
 
      fprintf(stderr, "  [+] Painting %s ... ", fname);
      while(1){
        off_t beg = Ftello(INPUT);
        if(fscanf(INPUT, "%"PRIu64":%"PRIu64"\n", &iPos, &ePos) != 2){
          Fseeko(INPUT, (off_t) beg, SEEK_SET);
          Chromosome(OUTPUT, Paint->width, GetPoint(fsize), Paint->cx,
          Paint->cy);
          if(nSeq > 0) 
            Paint->cx += DEFAULT_WIDTH + DEFAULT_SPACE;
          break;
          }
        else{
          Rect(OUTPUT, Paint->width, GetPoint(ePos-iPos+1), Paint->cx,
          Paint->cy + GetPoint(iPos), GetRgbColor(LEVEL_HUE));
          }
        }

      fprintf(stderr, "Done!\n");
      }
    }

  PrintFinal(OUTPUT);
  
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
