#ifndef PARAM_H_INCLUDED
#define PARAM_H_INCLUDED

#include "defs.h"
#include "cmodel.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct{
  U32    ctx;
  U32    den;
  U32    ir;
  U32    edits;
  U32    eDen;
  CModel *CM;
  }
ModelPar;

typedef struct{
  U8       help;
  U8       verbose;
  U8       force;
  U8       estim;
  U8       level;
  U32      col;
  U32      windowSize;
  U32      blockSize;
  double   gamma;
  double   threshold;
  U32      index;
  U32      nModels;
  U32      nThreads;
  U8       nFiles;
  U64      *size;
  char     **files;
  double   *matrix;
  uint8_t  *image;
  uint8_t  *labels;
  uint8_t  *output;
  uint32_t ref;
  }
Parameters;

typedef struct{
  uint32_t id;
  uint32_t tar;
  uint32_t ref;
  uint64_t min;
  ModelPar *model;
  }
Threads;

Parameters *P;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
