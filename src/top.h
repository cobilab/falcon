#ifndef TOP_H_INCLUDED
#define TOP_H_INCLUDED

#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  uint8_t  *name;
  double   value;
  }
VT;

typedef struct{
  uint32_t id;
  uint32_t size;
  VT       *V;
  }
TOP;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

TOP   *CreateTop(uint32_t);
void  DeleteTop(TOP *T);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
