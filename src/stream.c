#include <string.h>
#include "stream.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CREATE AUX STREAM
//
STREAM *CreateStream(uint64_t size){
  STREAM *S = (STREAM *) Calloc(1, sizeof(STREAM));
  S->size   = size;
  S->init   = size;
  S->bases  = (uint8_t *) Calloc(S->size, sizeof(uint8_t));
  S->bits   = (SPREC   *) Calloc(S->size, sizeof(SPREC  ));
  S->idx    = 0;
  return S;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE AUX STREAM
//
void UpdateStream(STREAM *S){
  if(++S->idx == S->size){
    S->size += S->init;
    S->bases = (uint8_t *) Realloc(S->bases, S->size * sizeof(uint8_t), S->init
               * sizeof(uint8_t));
    S->bits  = (SPREC   *) Realloc(S->bits,  S->size * sizeof(SPREC  ), S->init
               * sizeof(SPREC  ));
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// RESET AUX STREAM
// 
void ResetStream(STREAM *S){
  if(S->size != S->init){
    Free(S->bases);
    Free(S->bits);
    S->size  = S->init;
    S->bases = (uint8_t *) Calloc(S->size, sizeof(uint8_t));
    S->bits  = (SPREC   *) Calloc(S->size, sizeof(SPREC  ));
    }
  S->idx  = 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// REMOVE AUX STREAM
//
void RemoveStream(STREAM *S){
  Free(S->bases);
  Free(S->bits);
  Free(S);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
