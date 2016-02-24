#ifndef FILTERS_H_INCLUDED
#define FILTERS_H_INCLUDED

#include "defs.h"
#include "param.h"

#define W_HAMMING     0
#define W_HANN        1
#define W_BLACKMAN    2
#define W_RECTANGULAR 3

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void     WindowSizeAndDrop  (Param *, uint64_t);
float    *InitWinWeights    (int64_t, int32_t);
void     EndWinWeights      (float *);
void     FilterSequence     (char *, Param *, float *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
