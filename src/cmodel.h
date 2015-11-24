#ifndef CMODEL_H_INCLUDED
#define CMODEL_H_INCLUDED

#include "defs.h"
#include "buffer.h"

#define ARRAY_MODE            0
#define HASH_TABLE_MODE       1
#define HASH_TABLE_BEGIN_CTX  15
#define HASH_SIZE             33554471
#define MAX_COLLISIONS        10

#if defined(PREC32B)
  #define MAX_HASH_CTX        28 
#elif defined(PREC16B)
  #define MAX_HASH_CTX        20 
#else
  #define MAX_HASH_CTX        16
#endif

typedef U16  ACC;                  // Size of context counters for arrays
typedef U8   HCC;             // Size of context counters for hash tables
typedef U16  ENTMAX;                // Entry size (nKeys for each hIndex)
typedef HCC  HCCounters[4];

typedef struct{
  #if defined(PREC32B)
  U32        key;                         // The key stored in this entry
  #elif defined(PREC16B)
  U16        key;
  #else
  U8         key;
  #endif
  HCC        counters;           // "Small" counters: 2 bits for each one
  }
Entry;

typedef struct{
  ENTMAX     *index;                      // Number of keys in this entry
  Entry      **entries;              // The heads of the hash table lists
  uint32_t   maxC;
  uint32_t   maxH;
  }
HashTable;

typedef struct{
  ACC        *counters;
  }
Array;

typedef struct{
  uint32_t in;
  CBUF     *seq;      // BUFFER FOR EDITED SEQUENCE
  uint8_t  *mask;     // BUFFER FOR FAILS & HITS
  uint64_t idx;       // INDEX FOR UPDATE
  uint64_t idx2;      // AUXILIAR INDEX FOR UPDATE
  uint32_t threshold; // DISCARD ABOVE THIS VALUE
  uint32_t eDen;      // ALPHA DENOMINATOR FOR THIS MODEL
  }
Correct;

typedef struct{
  U32        ctx;                    // Current depth of context template
  U64        nPModels;            // Maximum number of probability models
  U32        alphaDen;                            // Denominator of alpha
  U32        maxCount;        // Counters /= 2 if one counter >= maxCount
  U64        multiplier;
  U8         ir;
  U8         ref;
  U32        mode;
  HashTable  hTable;
  Array      array;

  // INDEXES 
  U64        pModelIdx;
  U64        pModelIdxIR;
  // EDITS HANDLING:
  U32        edits;
  Correct    SUBS;
  }
CModel;

typedef struct{
  U32        *freqs;
  U32        sum;
  }
PModel;

typedef struct{
  double     *freqs;
  }
FloatPModel;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t         BestId               (uint32_t *, uint32_t);
void            HitSUBS              (CModel *);
void            FailSUBS             (CModel *);
void            FreeCModel           (CModel *);
void            FreeShadow           (CModel *);
inline void     GetPModelIdx         (U8 *, CModel *);
inline U8       GetPModelIdxIR       (U8 *, CModel *);
inline uint64_t GetPModelIdxCorr     (U8 *, CModel *, uint64_t);
void            CorrectCModelSUBS    (CModel *, PModel *, uint8_t);
PModel          *CreatePModel        (U32);
FloatPModel     *CreateFloatPModel   (U32);
void            RemovePModel         (PModel *);
void            RemoveFPModel        (FloatPModel *);
void            ResetCModelIdx       (CModel *);
void            ResetShadowModel     (CModel *);
void            UpdateCModelCounter  (CModel *, U32, U64);
CModel          *CreateCModel        (U32, U32, U32, U8, U32, U32, U32);
CModel          *CreateShadowModel   (CModel *);
inline void     ComputeMXProbs       (FloatPModel *, PModel *);
inline void     ComputePModel        (CModel *, PModel *, uint64_t, uint32_t);
inline void     ComputeWeightedFreqs (double, PModel *, FloatPModel *);
double          PModelSymbolLog      (PModel *, U32);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
