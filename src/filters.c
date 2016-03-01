#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include "filters.h"
#include "common.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static double Mean(FILTER *FIL, int64_t n){
  int64_t k, s;
  double  sum = 0, wSum = 0, tmp;

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
  int64_t n;

  for(n = 0 ; n < FIL->nEntries ; ++n){
    if(n % FIL->drop == 0){

      //TODO: SEGMENT HERE AND WRITE ONLY INIT AND END POSITIONS
      fprintf(OUT, "%"PRIu64"\t%.2lf\n", n, Mean(FIL, n));

      }
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

  FIL->nEntries = nEntries;
  FIL->entries  = (double *) Malloc(FIL->nEntries * sizeof(double)); 
  for(idx = 0 ; idx < FIL->nEntries ; ++idx){
    c = fgetc(INPUT);
    if(c == EOF || c == '\n'){
      fprintf(stderr, "  [x] Error: filtering symbols larger than size!");
      exit(1);
      }
    FIL->entries[idx] = (double) ((c-48)*0.25);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteEntries(FILTER *FIL){
  Free(FIL->entries);
  FIL->entries = NULL;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FILTER *CreateFilter(uint64_t size, uint64_t drop, uint8_t type, double 
threshold){
  FILTER *FIL    = (FILTER *) Calloc(1, sizeof(FILTER));
  FIL->size      = size;
  FIL->drop      = drop;
  FIL->type      = type;
  FIL->threshold = threshold;
  FIL->weights   = (double *) Malloc((2*FIL->size+1) * sizeof(double));
  InitWeights(FIL);
  FIL->entries   = NULL;
  return FIL;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteFilter(FILTER *FIL){
  Free(FIL->weights);
  if(FIL->entries != NULL)
    Free(FIL->entries);
  Free(FIL);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
