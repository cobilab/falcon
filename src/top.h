#ifndef TOP_H_INCLUDED
#define TOP_H_INCLUDED

#include "defs.h"
#include <stdlib.h>
#include <stdio.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  uint8_t  *name;
  double   value;
  uint64_t size;
  #ifdef LOCAL_SIMILARITY
  uint64_t iPos;
  uint64_t ePos;
  #endif
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
void       AddElement      (VT *, double, uint8_t *, uint64_t);
#ifdef LOCAL_SIMILARITY
void       AddElementWP    (VT *, double, uint8_t *, uint64_t, uint64_t, 
                           uint64_t);
#endif
int        SortByValue     (const void *, const void *);
void       UpdateTop       (double, uint8_t *, TOP *, uint64_t);
#ifdef LOCAL_SIMILARITY
void       UpdateTopWP     (double, uint8_t *, TOP *, uint64_t, uint64_t, 
                           uint64_t);
#endif
void       PrintTop        (FILE *, TOP *, uint32_t);
#ifdef LOCAL_SIMILARITY
void       PrintTopWP      (FILE *, TOP *, uint32_t);
#endif
void       PrintTopInfo    (TOP *, uint32_t);
#ifdef LOCAL_SIMILARITY
void       PrintTopInfoWP  (TOP *, uint32_t);
#endif
void       DeleteTop       (TOP *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
