#include "top.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

TOP *CreateTop(uint32_t size){
  uint32_t n;
  TOP *T  = (TOP *) Calloc(1, sizeof(TOP));
  T->size = size;
  T->V    = (VT  *) Calloc(size, sizeof(VT));
  for(n = 0 ; n < size ; ++n)
    T->V[n].name = (uint8_t *) Calloc(MAX_NAME, sizeof(uint8_t));
  return T;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteTop(TOP *T){
  uint32_t n;
  for(n = 0 ; n < T->size ; ++n)
    Free(T->V[n].name);
  Free(T->V);
  Free(T);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
