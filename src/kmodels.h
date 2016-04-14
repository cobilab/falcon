#ifndef KMODELS_H_INCLUDED
#define KMODELS_H_INCLUDED

#include "defs.h"
#include "buffer.h"

#define KARRAY_MODE            0
#define KHASH_TABLE_MODE       1
#define KHASH_TABLE_BEGIN_CTX  15
#define KHASH_SIZE             33554471
#define KMAX_COLLISIONS        10

#if defined(PREC32B)
  #define KMAX_HASH_CTX        28 
#elif defined(PREC16B)
  #define KMAX_HASH_CTX        20 
#else
  #define KMAX_HASH_CTX        16
#endif

typedef U16  ACC;             // Size of context counters for arrays
typedef U16  HCC;             // Size of context counters for hash tables
typedef U8   ENTMAX;          // Entry size (nKeys for each hIndex)

typedef struct{
  #if defined(PREC32B)
  U32        key;             // The key stored in this entry
  #elif defined(PREC16B)
  U16        key;
  #else
  U8         key;
  #endif
  U8         counters;
  }
KENTRY;

typedef struct{
  ENTMAX     *index;          // Number of keys in this entry
  KENTRY     **entries;       // The heads of the hash table lists
  uint32_t   maxC;
  uint32_t   maxH;
  }
KHASHTABLE;

typedef struct{
  ACC        *counters;
  }
KARRAY;

typedef struct{
  uint32_t   in;
  CBUF       *seq;            // BUFFER FOR EDITED SEQUENCE
  uint8_t    *mask;           // BUFFER FOR FAILS & HITS
  uint64_t   idx;             // INDEX FOR UPDATE
  uint64_t   idx2;            // AUXILIAR INDEX FOR UPDATE
  uint32_t   threshold;       // DISCARD ABOVE THIS VALUE
  uint32_t   eDen;            // ALPHA DENOMINATOR FOR THIS MODEL
  }
KCORRECT;

typedef struct{
  U32        ctx;             // Current depth of context template
  U64        nPModels;        // Maximum number of probability models
  U32        alphaDen;        // Denominator of alpha
  U32        maxCount;        // Counters /= 2 if one counter >= maxCount
  U64        multiplier;
  U8         ir;
  U8         ref;
  U32        mode;
  KHASHTABLE hTable;
  KARRAY     array;
  U64        idx;
  U64        idx2Up;
  U64        idxIR;
  U32        edits;
  KCORRECT   SUBS;
  }
KMODEL;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t         BestKId              (uint32_t *, uint32_t);
void            HitKSUBS             (KMODEL *);
void            FailKSUBS            (KMODEL *);
void            FreeKModel           (KMODEL *);
void            FreeKShadow          (KMODEL *);
inline void     GetKIdx              (U8 *, KMODEL *);
inline U8       GetKIdxIR            (U8 *, KMODEL *);
inline uint64_t GetKModelIdxCorr     (U8 *, KMODEL *, uint64_t);
void            CorrectKModelSUBS    (KMODEL *, PModel *, uint8_t);
void            ResetKModelIdx       (KMODEL *);
void            ResetKShadowModel    (KMODEL *);
void            UpdateKModelCounter  (KMODEL *, U32, U64);
KMODEL          *CreateKModel        (U32, U32, U32, U8, U32, U32, U32);
KMODEL          *CreateKShadowModel  (KMODEL *);
inline void     ComputeKPModel       (KMODEL *, PModel *, uint64_t, uint32_t);
void            CorrectKXModels      (KMODEL **, PModel **, uint8_t);    

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
