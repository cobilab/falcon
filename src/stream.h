#ifndef STREAM_H_INCLUDED
#define STREAM_H_INCLUDED

#include "defs.h"

#define DEF_STREAM_SIZE 2000000

typedef double SPREC;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  uint8_t  *bases;
  SPREC    *bits;
  uint64_t size;
  uint64_t init;
  uint64_t idx;
  }
STREAM;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

STREAM      *CreateStream  (uint64_t);
void        UpdateStream   (STREAM *, uint8_t, SPREC);
void        ResetStream    (STREAM *);
void        RemoveStream   (STREAM *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
