#ifndef FILTERS_H_INCLUDED
#define FILTERS_H_INCLUDED

#include "defs.h"
#include "param.h"

#define W_HAMMING     0
#define W_HANN        1
#define W_BLACKMAN    2
#define W_RECTANGULAR 3

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  int64_t  size;
  int64_t  drop;
  int64_t  nEntries;
  double   *entries;
  double   *weights;
  uint8_t  type;
  }
FILTER;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FILTER   *CreatFilter       (uint64_t, uint64_t, uint8_t);
void     InitWeights        (FILTER *);
void     DeleteFilter       (FILTER *);


void     FilterSequence     (char *, Param *, float *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
