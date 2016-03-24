#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include "filters.h"
#include "common.h"
#include "mem.h"
#include "models.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static ENTP Mean(FILTER *FIL, int64_t n){
  int64_t k, s;
  ENTP sum = 0, wSum = 0, tmp;
  for(k = -FIL->size ; k <= FIL->size ; ++k){
    s = n+k;
    if(s >= 0 && s < FIL->nEntries){
      sum  += (tmp= FIL->weights[FIL->size+k]) * FIL->entries[s];
      wSum += tmp;
      }
    }
  return sum/wSum;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void FilterStream(FILTER *FIL, FILE *OUT){
  int64_t  n = 1;
  ENTP     val = Mean(FIL, 0);
  int      region = val < FIL->threshold ? LOW_REGION : HIGH_REGION, cmp = 0;
  uint64_t initPosition = 1;
  uint64_t lastPosition = n;
  for(n = 1 ; n < FIL->nEntries ; ++n){
    if(n % FIL->drop == 0){
      if((val = Mean(FIL, n)) >= FIL->threshold){
        if(region == LOW_REGION){
          region = HIGH_REGION;
          cmp = SelfSimilarity(FIL->bases, initPosition, n);
          fprintf(OUT, "%"PRIu64":%"PRIu64"\t%u\n", initPosition, n, cmp);
          }
        }
      else{ // val < threshold ====> LOW_REGION
        if(region == HIGH_REGION){
          region       = LOW_REGION;
          initPosition = n;
          }
        }
      lastPosition = n;
      }
    }
  if(region == LOW_REGION){
    cmp = SelfSimilarity(FIL->bases, initPosition, n);
    fprintf(OUT, "%"PRIu64":%"PRIu64"\t%u\n", initPosition, lastPosition, cmp);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void InitWeights(FILTER *FIL){
  int64_t k, sizedb = 2 * FIL->size + 1;
  switch(FIL->type){
    case W_HAMMING:
      for(k = -FIL->size ; k <= FIL->size ; ++k)
        FIL->weights[FIL->size+k] = 0.54 + 0.46 * cos((2 * M_PI * k) / sizedb);
    break;
    case W_HANN:
      for(k = -FIL->size ; k <= FIL->size ; ++k)
        FIL->weights[FIL->size+k] = 0.5 * (1 + cos((2 * M_PI * k) / sizedb));
    break;
    case W_BLACKMAN:
      for(k = -FIL->size ; k <= FIL->size ; ++k)
        FIL->weights[FIL->size+k] = 0.42 + 0.5 * cos((2 * M_PI * k) / sizedb) +
        0.08 * cos((4 * M_PI * k) / sizedb);
    break;
    case W_RECTANGULAR:
      for(k = -FIL->size ; k <= FIL->size ; ++k)
        FIL->weights[FIL->size+k] = 1;
    break;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void InitEntries(FILTER *FIL, uint64_t nEntries, FILE *INPUT){
  int c;
  uint64_t idx;
  SymValue *SM  = Calloc(1, sizeof(SymValue));
  FIL->nEntries = nEntries;
  FIL->entries  = (ENTP    *) Malloc(FIL->nEntries * sizeof(ENTP   )); 
  FIL->bases    = (uint8_t *) Malloc(FIL->nEntries * sizeof(uint8_t)); 
  for(idx = 0 ; idx < FIL->nEntries ; ++idx){
    c = fgetc(INPUT);
    if(c == EOF || c == '\n'){
      fprintf(stderr, "  [x] Error: filtering symbols larger than size!");
      exit(1);
      }
    UnPackByte(SM, c);
    FIL->bases  [idx] = (uint8_t) SM->sym;
    FIL->entries[idx] = (ENTP)    SM->value * 0.25;
    }
  Free(SM);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteEntries(FILTER *FIL){
  Free(FIL->entries);
  Free(FIL->bases);
  FIL->entries = NULL;
  FIL->bases   = NULL;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FILTER *CreateFilter(uint64_t size, uint64_t drop, uint8_t type, double 
threshold){
  FILTER *FIL    = (FILTER *) Calloc(1, sizeof(FILTER));
  FIL->size      = size;
  FIL->drop      = drop;
  FIL->type      = type;
  FIL->threshold = threshold;
  FIL->weights   = (ENTP *) Malloc((2*FIL->size+1) * sizeof(ENTP));
  InitWeights(FIL);
  FIL->entries   = NULL;
  FIL->bases     = NULL;
  return FIL;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteFilter(FILTER *FIL){
  Free(FIL->weights);
  if(FIL->entries != NULL)
    Free(FIL->entries);
  if(FIL->bases   != NULL)
    Free(FIL->bases);
  Free(FIL);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
