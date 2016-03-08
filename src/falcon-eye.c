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
  char **p = *&argv, fname[MAX_NAME];
  FILE *OUTPUT = NULL, *INPUT = NULL;
  int sym;
  double fvalue;
  uint32_t n;
  uint64_t maxSize = 0, fsize, iPos, ePos, nSeq;
  Painter *Paint;
  COLORS *CLR;
  
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
  PEYE->space     = ArgsDouble (DEFAULT_SPACE,   p, argc, "-s");
  PEYE->start     = ArgsDouble (0.35,            p, argc, "-i");
  PEYE->rotations = ArgsDouble (1.50,            p, argc, "-r");
  PEYE->hue       = ArgsDouble (1.92,            p, argc, "-u");
  PEYE->gamma     = ArgsDouble (0.50,            p, argc, "-g");
  PEYE->lowerSimi = ArgsDouble (0.00,            p, argc, "-sl");
  PEYE->upperSimi = ArgsDouble (100.00,          p, argc, "-su");
  PEYE->lowerSize = ArgsNum64  (1,               p, argc, "-dl", 1, 9999999999);
  PEYE->upperSize = ArgsNum64  (9999999999,      p, argc, "-du", 1, 9999999999);
  PEYE->enlarge   = ArgsNum64  (0,               p, argc, "-e",  0, 9999999999);
  PEYE->output    = ArgsFileGen(p, argc, "-o", "femap", ".svg");

  if(!PEYE->force) 
    FAccessWPerm(PEYE->output);
  OUTPUT = Fopen(PEYE->output, "w");

  fprintf(stderr, "\n");
  if(PEYE->verbose){
    PrintArgsEye(PEYE);
    }

  fprintf(stderr, "==[ PROCESSING ]====================\n");
  TIME *Time = CreateClock(clock());

  // TODO: OPTION TO IGNORE SCALE

  CLR = (COLORS *) Calloc(1, sizeof(COLORS));
  CLR->start     = PEYE->start;
  CLR->rotations = PEYE->rotations;
  CLR->hue       = PEYE->hue;
  CLR->gamma     = PEYE->gamma;

  INPUT = Fopen(argv[argc-1], "r"); 
  nSeq = 0;
  while((sym = fgetc(INPUT)) != EOF){
    if(sym == '$'){
      if(fscanf(INPUT, "\t%lf\t%"PRIu64"\t%s\n", &fvalue, &fsize, fname) != 3){
        fprintf(stderr, "  [x] Error: unknown type of file!\n");
        exit(1);
        }
      if(fsize > PEYE->upperSize ||  fsize < PEYE->lowerSize ||
        fvalue > PEYE->upperSimi || fvalue < PEYE->lowerSimi){
        continue;
        }

      if(fsize > maxSize)
        maxSize = fsize;
      ++nSeq;
      }
    }
  rewind(INPUT);

  SetScale(maxSize);
  Paint = CreatePainter(GetPoint(maxSize), PEYE->width, PEYE->space, "#ffffff");

  PrintHead(OUTPUT, (2 * DEFAULT_CX) + (((Paint->width + PEYE->space) *
  nSeq) - PEYE->space), Paint->size + EXTRA + Paint->width);
  Rect(OUTPUT, (2 * DEFAULT_CX) + (((Paint->width + PEYE->space) * nSeq) - 
  PEYE->space), Paint->size + EXTRA + Paint->width, 0, 0, "#ffffff");

  // PRINT HEATMAP SCALE
  uint32_t size = DEFAULT_CX + Paint->width;
  for(n = 0 ; n < size ; ++n){
    char color[12];
    Rect(OUTPUT, Paint->width, 1, DEFAULT_CX - (Paint->width*2), 
    Paint->cy + n, HeatMapColor(((double) n / size), color, CLR));
    }
  Text(OUTPUT, DEFAULT_CX-(Paint->width*2 + 14), Paint->cy+13,   "+");
  Text(OUTPUT, DEFAULT_CX-(Paint->width*2 + 12), Paint->cy+size, "-");

  while((sym = fgetc(INPUT)) != EOF){

    if(sym == '$'){
      if(fscanf(INPUT, "\t%lf\t%"PRIu64"\t%s\n", &fvalue, &fsize, fname) != 3){
        fprintf(stderr, "  [x] Error: unknown type of file!\n");
        exit(1);
        }

      // SKIPS: FILTERED ATTRIBUTES
      if(fsize > PEYE->upperSize ||  fsize < PEYE->lowerSize ||
        fvalue > PEYE->upperSimi || fvalue < PEYE->lowerSimi){
        while(fscanf(INPUT, "%"PRIu64":%"PRIu64"\n", &iPos, &ePos) == 2)
          ;
        continue;
        }

      char tmpTxt[MAX_NAME], color[12];
      sprintf(tmpTxt, "%u", (unsigned) fvalue);
      if(fvalue < 10)
        Text(OUTPUT, (Paint->cx+Paint->width/2)-4, Paint->cy-10, tmpTxt);
      else
        Text(OUTPUT, (Paint->cx+Paint->width/2)-9, Paint->cy-10, tmpTxt);
      RectWithBorder(OUTPUT, Paint->width, Paint->width, Paint->cx, Paint->cy, 
      HeatMapColor(BoundDouble(0.0, 1-fvalue/100.0, 1.0), color, CLR));

      Paint->cy += Paint->width + Paint->space;
 
      fprintf(stderr, "  [+] Painting %s ... ", fname);
      while(1){
        off_t beg = Ftello(INPUT);
        if(fscanf(INPUT, "%"PRIu64":%"PRIu64"\n", &iPos, &ePos) != 2){
          Fseeko(INPUT, (off_t) beg, SEEK_SET);
          Chromosome(OUTPUT, Paint->width, GetPoint(fsize), Paint->cx, 
          Paint->cy);
          if(nSeq > 0) 
            Paint->cx += Paint->width + Paint->space;
          break;
          }
        else{
          if(PEYE->enlarge == 0){
            Rect(OUTPUT, Paint->width, GetPoint(ePos-iPos+1), Paint->cx,
            Paint->cy + GetPoint(iPos), /*HeatMapColor(0, color, CLR)*/ 
            GetRgbColor(LEVEL_HUE));
            }
          else{
            Rect(OUTPUT, Paint->width, GetPoint(ePos-iPos+1+PEYE->enlarge), 
            Paint->cx, Paint->cy + GetPoint(iPos), GetRgbColor(LEVEL_HUE));
            }
          }
        }

      Paint->cy -= Paint->width + Paint->space;
      fprintf(stderr, "Done!\n");
      }
    }

  PrintFinal(OUTPUT);
  
  if(!OUTPUT) fclose(OUTPUT);
  if(!INPUT)  fclose(INPUT);
  Free(CLR);

  StopTimeNDRM(Time, clock());
  fprintf(stderr, "\n");

  fprintf(stderr, "==[ STATISTICS ]====================\n");
  StopCalcAll(Time, clock());
  fprintf(stderr, "\n");

  RemoveClock(Time);
  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
