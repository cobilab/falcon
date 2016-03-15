#ifndef FILTERS_H_INCLUDED
#define FILTERS_H_INCLUDED

#include "defs.h"
#include "param.h"

#define W_HAMMING     0
#define W_HANN        1
#define W_BLACKMAN    2
#define W_RECTANGULAR 3
#define LOW_REGION    0
#define HIGH_REGION   1

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  int64_t  size;
  int64_t  drop;
  int64_t  nEntries;
  double   *entries; // TODO: SET PRECISION WITH MACRO
  uint8_t  *bases;
  double   *weights;
  double   threshold;
  uint8_t  type;
  }
FILTER;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FILTER   *CreateFilter      (uint64_t, uint64_t, uint8_t, double);
void     InitWeights        (FILTER *);
void     InitEntries        (FILTER *, uint64_t, FILE *);
void     DeleteFilter       (FILTER *);
void     DeleteEntries      (FILTER *);
void     FilterStream       (FILTER *, FILE *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
