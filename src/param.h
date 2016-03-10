#ifndef PARAM_H_INCLUDED
#define PARAM_H_INCLUDED

#include "defs.h"
#include "models.h"
#include "top.h"

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
  #ifdef LOCAL_SIMILARITY
  U8       local;
  char     *outLoc;
  #endif
  U32      sample;
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
  TOP      *top;
  uint8_t  *image;
  char     *output;
  char     *base;
  }
Parameters;

typedef struct{
  uint32_t id;
  uint32_t tar;
  uint32_t ref;
  uint64_t min;
  TOP      *top;
  ModelPar *model;
  }
Threads;

typedef struct{
  char     **names;
  uint32_t nFiles;
  }
SFILES;

typedef struct{
  U8       help;
  U8       verbose;
  U8       force;
  U8       nFiles;
  double   start;
  double   rotations;
  double   hue;
  double   gamma;
  double   width;
  double   space;
  double   upperSimi;
  double   lowerSimi;
  int64_t  upperSize;
  int64_t  lowerSize;
  int64_t  windowSize;
  int      windowType;
  int64_t  sampling;
  double   threshold;
  int64_t  enlarge;
  U8       showScale;
  U8       showNames;
  U8       sameScale;
  char     *output;
  }
EYEPARAM;

typedef struct{
  uint8_t  verbose;
  uint8_t  disk;
  char     *output;
  SFILES   *ref;
  SFILES   *tar;
  uint32_t context;
  uint32_t inverse;
  uint64_t **size;
  uint64_t *chrSize;
  int64_t  subsamp;
  int64_t  window;
  int64_t  ratio;
  uint8_t  bloom;
  uint64_t bSize;
  uint32_t bHashes;
  uint64_t max;
  double   threshold;
  }
Param;

Parameters *P;
EYEPARAM   *PEYE;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
