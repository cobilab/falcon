#ifndef TOP_H_INCLUDED
#define TOP_H_INCLUDED

#include "defs.h"
#include <stdlib.h>
#include <stdio.h>

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

TOP        *CreateTop      (uint32_t);
void       CopyStringPart  (uint8_t *, uint8_t *);
void       AddElement      (VT *, double, uint8_t *);
int        SortByValue     (const void *, const void *);
void       UpdateTop       (double, uint8_t *, TOP *);
void       PrintTop        (FILE *, TOP *, uint32_t);
void       DeleteTop       (TOP *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
