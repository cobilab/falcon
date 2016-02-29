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

static float Mean(float *ent, int64_t nEnt, int64_t n, int64_t M, float *w){
  int64_t k, s;
  float   sum = 0, wSum = 0, tmp;

  for(k = -M ; k <= M ; ++k){
    s = n+k;
    if(s >= 0 && s < nEnt){
      sum  += (tmp=w[M+k])*ent[s];
      wSum += tmp;
      }
    }
  
  return sum/wSum;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void FilterSequence(char *fName, Param *P, float *w){
  FILE    *Reader  = NULL, *Writter = NULL;
  float   *entries = NULL;
  int64_t nEntries, n, M, drop, k;
  char    *fNameOut, *buffer;

  M        = P->window;
  drop     = P->subsamp + 1;
  Reader   = Fopen(fName, "r");
  entries  = (float *) Malloc(BUFFER_SIZE * sizeof(float));
  buffer   = (char  *) Malloc(BUFFER_SIZE * sizeof(char ));
  nEntries = 0;

  while((k = fread(buffer, sizeof(char), BUFFER_SIZE, Reader))){
    for(n = 0 ; n < k ; ++n)
      switch(buffer[n]){
        case '0': entries[nEntries++] = 0.0; break;
        case '1': entries[nEntries++] = 1.0; break;
        default:  entries[nEntries++] = 1.0; break; // UNKNOWN SYMBOLS
        }
    entries = (float *) Realloc(entries, (nEntries+k) * sizeof(float), 
    sizeof(float) * k);
    }
  fclose(Reader);

  if(P->verbose == 1)
    fprintf(stderr, "Got %"PRIu64" entries from file\n", nEntries);

  fNameOut = ReplaceSubStr(fName, ".oxch", ".fil");
  Writter  = Fopen(fNameOut, "w");

  for(n = 0 ; n < nEntries ; ++n)
    if(n % drop == 0)
      fprintf(Writter, "%"PRIu64"\t%f\n", n, Mean(entries, nEntries, n, M, w));

  Free(entries);
  fclose(Writter);
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

FILTER *CreatFilter(uint64_t size, uint64_t drop, uint8_t type){
  FILTER *FIL  = (FILTER *) Calloc(1, sizeof(FILTER));
  FIL->size    = size;
  FIL->drop    = drop;
  FIL->type    = type;
  FIL->weights = (double *) Malloc((2*FIL->size+1) * sizeof(double));
  InitWeights(FIL);
  return FIL;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteFilter(FILTER *FIL){
  Free(FIL->weights);
  Free(FIL->entries);
  Free(FIL);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
