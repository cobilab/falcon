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

void WindowSizeAndDrop(Param *P, uint64_t size){
  if(DEFAULT_WINDOW != -1) return;  
  P->subsamp = size / DEFAULT_SAMPLE_RATIO;
  if(size < DEFAULT_SAMPLE_RATIO)
    P->subsamp = 1;
  P->window = (P->subsamp-1) * SUBSAMPLE_RATIO;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

float *InitWinWeights(int64_t M, int32_t type){
  float  *w = (float *) Malloc((2 * M + 1) * sizeof(float));
  int64_t k;

  switch(type){
    case W_HAMMING: for(k = -M ; k <= M ; ++k) w[M+k] = 0.54 + 0.46 * cos((2 *
    M_PI * k) / (2 * M + 1)); break;
    case W_HANN: for(k = -M ; k <= M ; ++k) w[M+k] = 0.5 * (1 + cos((2 * M_PI
    * k) / (2 * M + 1))); break;
    case W_BLACKMAN: for(k = -M ; k <= M ; ++k) w[M+k] = 0.42 + 0.5 * cos((2 *
    M_PI * k) / (2 * M + 1)) + 0.08 * cos((4 * M_PI * k) / (2 * M+1)); break;
    case W_RECTANGULAR: for(k = -M ; k <= M ; ++k) w[M+k] = 1; break;
    }

  return w;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void EndWinWeights(float *w){
  Free(w);
  }

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

