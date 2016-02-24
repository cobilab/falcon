#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "segment.h"
#include "common.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SegmentSequence(char *fName, Param *P){
  FILE     *Reader = NULL , *Writter = NULL;
  float    val, threshold;
  uint64_t pos, initPosition, lastPosition;
  int32_t  region;
  char     *fNameOut;

  threshold    = (float) P->threshold; 
  Reader       = Fopen(fName, "r");
  fNameOut     = ReplaceSubStr(fName, ".fil", ".seg");
  Writter      = Fopen(fNameOut, "w");

  if(fscanf(Reader, "%"PRIu64"\t%f", &pos, &val) == 2)
    region = val < threshold ? LOW_REGION : HIGH_REGION;
  else{
    fprintf(stderr, "Error: unknown format\n");
    exit(1);
    }
  initPosition = 1;
  lastPosition = pos;

  while(fscanf(Reader, "%"PRIu64"\t%f", &pos, &val) == 2){
    if(val >= threshold){ 
      if(region == LOW_REGION){
        region = HIGH_REGION;
        fprintf(Writter, "%"PRIu64"\t%"PRIu64"\n", initPosition, pos);
        }
      }
    else{ // val < threshold ====> LOW_REGION
      if(region == HIGH_REGION){
        region = LOW_REGION;
        initPosition = pos;
        }
      }
    lastPosition = pos;
    }

  if(region == LOW_REGION)
    fprintf(Writter, "%"PRIu64"\t%"PRIu64"\n", initPosition, lastPosition);

  fclose(Reader);
  fclose(Writter);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

