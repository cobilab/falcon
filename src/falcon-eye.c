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
  uint32_t n, tmp, maxName, extraLength, cmp;
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

  PEYE->verbose    = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");
  PEYE->force      = ArgsState  (DEFAULT_FORCE,   p, argc, "-F");
  PEYE->width      = ArgsDouble (DEFAULT_WIDTH,   p, argc, "-w");
  PEYE->space      = ArgsDouble (DEFAULT_SPACE,   p, argc, "-s");
  PEYE->showScale  = ArgsState  (DEFAULT_SHOWS,   p, argc, "-ss");
  PEYE->showNames  = ArgsState  (DEFAULT_NAMES,   p, argc, "-sn");
  PEYE->sameScale  = ArgsState  (DEFAULT_RSCAL,   p, argc, "-rs");
  PEYE->start      = ArgsDouble (0.35,            p, argc, "-i");
  PEYE->rotations  = ArgsDouble (1.50,            p, argc, "-r");
  PEYE->hue        = ArgsDouble (1.92,            p, argc, "-u");
  PEYE->gamma      = ArgsDouble (0.50,            p, argc, "-g");
  PEYE->proportion = ArgsDouble (500,             p, argc, "-p");
  PEYE->lowerSimi  = ArgsDouble (0.00,            p, argc, "-sl");
  PEYE->upperSimi  = ArgsDouble (100.00,          p, argc, "-su");
  PEYE->lowerSize  = ArgsNum64  (1,               p, argc, "-dl", 1, 
  9999999999);
  PEYE->upperSize  = ArgsNum64  (9999999999,      p, argc, "-du", 1, 
  9999999999);
  PEYE->enlarge    = ArgsNum64  (0,               p, argc, "-e",  0, 
  9999999999);
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

  // TODO: OPTION TO IGNORE SCALE AND SET THEM AT SAME SIZE

  CLR = (COLORS *) Calloc(1, sizeof(COLORS));
  CLR->start     = PEYE->start;
  CLR->rotations = PEYE->rotations;
  CLR->hue       = PEYE->hue;
  CLR->gamma     = PEYE->gamma;

  INPUT = Fopen(argv[argc-1], "r"); 
  nSeq = 0;
  maxName = 0;
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

      if((tmp = strlen(fname)) > maxName)
        maxName = tmp;
     
      ++nSeq;
      }
    }
  rewind(INPUT);

  Paint = CreatePainter(maxSize, PEYE->width, PEYE->space, PEYE->proportion, 
  "#ffffff");

  extraLength = 0;
  if(PEYE->showNames == 1){
    extraLength = 10 * maxName; // LETTER SIZE * MAXNAME
    Paint->cy += extraLength;
    }

  if(PEYE->showScale == 1)
    nSeq += 2;

  PrintHead(OUTPUT, (2 * DEFAULT_CX) + (((Paint->width + PEYE->space) * nSeq) - 
  PEYE->space), Paint->size + EXTRA + Paint->width + extraLength);
  Rect(OUTPUT, (2 * DEFAULT_CX) + (((Paint->width + PEYE->space) * nSeq) - 
  PEYE->space), Paint->size + EXTRA + Paint->width + extraLength, 0, 0, 
  "#ffffff");

  if(PEYE->showScale == 1){
    nSeq -= 2;
    Paint->cx += (2 * Paint->width + PEYE->space);
    // PRINT HEATMAP SCALE
    uint32_t size = 4 * Paint->width;
    for(n = 0 ; n < size ; ++n){
      char color[12];
      Rect(OUTPUT, Paint->width, 1, Paint->cx - (Paint->width*2), 
      Paint->cy + n, HeatMapColor(((double) n / size), color, CLR));
      }
    Text(OUTPUT, Paint->cx-(Paint->width*2 + 14), Paint->cy+18,     "+");
    Text(OUTPUT, Paint->cx-(Paint->width*2 + 12), Paint->cy+size-6, "-");

    // HIGH COMPLEX SCALE
    Rect(OUTPUT, Paint->width, Paint->width, Paint->cx - (Paint->width*2), 
    Paint->cy + Paint->width + size, GetRgbColor(HIGH_COMPLEX));
    Text(OUTPUT, Paint->cx-(Paint->width*2 + 14), (Paint->cy+Paint->width+size)
    +(Paint->width/2)+5, "+");
    // MEDIUMH COMPLEX SCALE
    Rect(OUTPUT, Paint->width, Paint->width, Paint->cx - (Paint->width*2),
    Paint->cy + 2*Paint->width + size, GetRgbColor(MEDIUMH_COMPLEX));
    // MEDIUML COMPLEX SCALE
    Rect(OUTPUT, Paint->width, Paint->width, Paint->cx - (Paint->width*2),
    Paint->cy + 3*Paint->width + size, GetRgbColor(MEDIUML_COMPLEX));
    // LOW COMPLEX SCALE
    Rect(OUTPUT, Paint->width, Paint->width, Paint->cx - (Paint->width*2), 
    Paint->cy + 4*Paint->width + size, GetRgbColor(LOW_COMPLEX));
    Text(OUTPUT, Paint->cx-(Paint->width*2+12), (Paint->cy+4*Paint->width+size)
    +(Paint->width/2)+4, "-");
    }

  while((sym = fgetc(INPUT)) != EOF){

    if(sym == '$'){
      if(fscanf(INPUT, "\t%lf\t%"PRIu64"\t%s\n", &fvalue, &fsize, fname) != 3){
        fprintf(stderr, "  [x] Error: unknown type of file!\n");
        exit(1);
        }

      // TODO: CHECK BEST FOR EACH FNAME:
/*      if(ParseFromOther(fname) == 1){  // IF PRESENT
        while(fscanf(INPUT, "%"PRIu64":%"PRIu64"\t%u\n", &iPos, &ePos, &cmp) == 3)
          ;
        continue;
        }
      else{ // IF NOT PRESENT
        if(fvalue < List){
          continue
        }
*/

      // SKIPS: FILTERED ATTRIBUTES
      if(fsize > PEYE->upperSize ||  fsize < PEYE->lowerSize ||
        fvalue > PEYE->upperSimi || fvalue < PEYE->lowerSimi){
        while(fscanf(INPUT, "%"PRIu64":%"PRIu64"\t%u\n", &iPos, &ePos, &cmp) 
        == 3)
          ;
        continue;
        }

      if(PEYE->showNames == 1){  // PRINT NAMES 90D
        Text90d(OUTPUT, -(Paint->cy-32), Paint->cx+Paint->width-
        (Paint->width/2.0)+10, fname);
        }

      char tmpTxt[MAX_NAME], color[12];
      if(fvalue < 10){
        sprintf(tmpTxt, "%.1lf", fvalue);
        Text(OUTPUT, (Paint->cx+Paint->width/2)-12, Paint->cy-10, tmpTxt);
        }
      else{
        sprintf(tmpTxt, "%u", (unsigned) fvalue);
        Text(OUTPUT, (Paint->cx+Paint->width/2)-9, Paint->cy-10, tmpTxt);
        }
      RectWithBorder(OUTPUT, Paint->width, Paint->width, Paint->cx, Paint->cy, 
      HeatMapColor(BoundDouble(0.0, 1-fvalue/100.0, 1.0), color, CLR));

      Paint->cy += Paint->width + Paint->space;
 
      fprintf(stderr, "  [+] Painting %s ... ", fname);
      while(1){
        off_t beg = Ftello(INPUT);
        if(fscanf(INPUT, "%"PRIu64":%"PRIu64"\t%u\n", &iPos, &ePos, &cmp) != 3){
          Fseeko(INPUT, (off_t) beg, SEEK_SET);
          Chromosome(OUTPUT, Paint->width, GetPoint(Paint, fsize), Paint->cx, 
          Paint->cy);
          if(nSeq > 0) 
            Paint->cx += Paint->width + Paint->space;
          break;
          }
        else{
          int color = 0;
          switch(cmp){
            case 0: color = LOW_COMPLEX;     break;
            case 1: color = MEDIUML_COMPLEX; break;
            case 2: color = MEDIUMH_COMPLEX; break;
            case 3: color = HIGH_COMPLEX;    break;
            default: color = 0;
            }
          if(PEYE->enlarge == 0){
            Rect(OUTPUT, Paint->width, GetPoint(Paint, ePos-iPos+1), Paint->cx,
            Paint->cy + GetPoint(Paint, iPos), GetRgbColor(color));
            }
          else{
            Rect(OUTPUT, Paint->width, GetPoint(Paint, ePos-iPos+1+PEYE->enlarge), 
            Paint->cx, Paint->cy + GetPoint(Paint, iPos), GetRgbColor(color));
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
