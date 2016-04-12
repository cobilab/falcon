#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "defs.h"
#include "mem.h"
#include "common.h"
#include "kmodels.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static uint64_t ZHASH(uint64_t z){
  z = (~z) + (z << 21);
  z = z    ^ (z >> 24);
  z = (z   + (z << 3)) + (z << 8);
  z = z    ^ (z >> 14);
  z = (z   + (z << 2)) + (z << 4);
  z = z    ^ (z >> 28);
  z = z    + (z << 31);
  return z;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void InitKHashTable(KMODEL *M, U32 c){ 
  uint32_t k;
  M->hTable.maxC    = c;
  M->hTable.index   = (ENTMAX  *) Calloc(KHASH_SIZE, sizeof(ENTMAX));
  M->hTable.entries = (KENTRY **) Calloc(KHASH_SIZE, sizeof(KENTRY *));
  for(k = 0 ; k < KHASH_SIZE ; ++k)
    M->hTable.entries[k] = (KENTRY *) Calloc(M->hTable.maxC, sizeof(KENTRY));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void FreeKModel(KMODEL *M){
  U32 k;
  if(M->mode == HASH_TABLE_MODE){
    for(k = 0 ; k < HASH_SIZE ; ++k)
      Free(M->hTable.entries[k]);
    Free(M->hTable.entries);
    Free(M->hTable.index);
    }
  else // TABLE_MODE
    Free(M->array.counters);
  if(M->edits != 0){
    Free(M->SUBS.mask);
    RemoveCBuffer(M->SUBS.seq);
    }
  Free(M);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void FreeKShadow(KMODEL *M){
  if(M->edits != 0){
    Free(M->SUBS.mask);
    RemoveCBuffer(M->SUBS.seq);
    }
  Free(M);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void InitKArray(KMODEL *M){
  M->array.counters = (ACC *) Calloc(M->nPModels<<2, sizeof(ACC));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void InsertKey(KHASHTABLE *H, U32 hi, U64 idx, U8 s){

  if(++H->index[hi] == H->maxC)
    H->index[hi] = 0;

  #if defined(PREC32B)
  H->entries[hi][H->index[hi]].key = (U32)(idx&0xffffffff);
  #elif defined(PREC16B)
  H->entries[hi][H->index[hi]].key = (U16)(idx&0xffff);
  #else
  H->entries[hi][H->index[hi]].key = (U8) (idx&0xff);
  #endif  
  H->entries[hi][H->index[hi]].counters = 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void GetFreqsFromKHCC(HCC c, uint32_t a, PModel *P){
  P->sum  = (P->freqs[0] = a * ( c &  0x0f) + 1);
  P->sum += (P->freqs[1] = a * ((c & (0x0f<<4 ))>>4)  + 1);
  P->sum += (P->freqs[2] = a * ((c & (0x0f<<8 ))>>8)  + 1);
  P->sum += (P->freqs[3] = a * ((c & (0x0f<<12))>>12) + 1);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GetKHCCounters(KHASHTABLE *H, U64 key, PModel *P, uint32_t a){
  U32 n, hIndex = key % KHASH_SIZE;
  #if defined(PREC32B)
  U32 b = key & 0xffffffff;
  #elif defined(PREC16B)
  U16 b = key & 0xffff;
  #else
  U8  b = key & 0xff;
  #endif

  U32 pos = H->index[hIndex];
  // FROM INDEX-1 TO 0
  for(n = pos+1 ; n-- ; ){
    if(H->entries[hIndex][n].key == b){
      GetFreqsFromKHCC(H->entries[hIndex][n].counters, a, P);
      return;
      }
    }
  // FROM MAX_COLISIONS TO INDEX
  for(n = (H->maxC-1) ; n > pos ; --n){
    if(H->entries[hIndex][n].key == b){
      GetFreqsFromKHCC(H->entries[hIndex][n].counters, a, P);
      return;
      }
    }

  // TODO: MAKE THIS ALREADY DONE!
  P->freqs[0] = 1;
  P->freqs[1] = 1;
  P->freqs[2] = 1;
  P->freqs[3] = 1;
  P->sum      = 4; 
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateKModelCounter(KMODEL *M, U32 sym, U64 idx){
  U32 n;
  ACC *AC;

  if(M->mode == KHASH_TABLE_MODE){
    U16 sc;
    U32 hIndex = (idx = ZHASH(idx)) % KHASH_SIZE;
    #if defined(PREC32B)
    U32 b = idx & 0xffffffff;
    #elif defined(PREC16B)
    U16 b = idx & 0xffff;
    #else
    U8  b = idx & 0xff;
    #endif

    for(n = 0 ; n < M->hTable.maxC ; ++n)
      if(M->hTable.entries[hIndex][n].key == b){
        if((sc = M->hTable.entries[hIndex][n].counters) == 255)
          return;
        ++sc;
        return;
        }

    InsertKey(&M->hTable, hIndex, b, sym); // KEY NOT FOUND: WRITE ON OLDEST
    }
  else{
    AC = &M->array.counters[idx];
    if(++AC == M->maxCount){  
      for(n = 0 ; n < 4 ; ++n){ // RENORMALIZE ALL
        M->array.counters[idx-sym+n] >>= 1;
        }
      }
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

KMODEL *CreateKModel(U32 ctx, U32 aDen, U32 ir, U8 ref, U32 col, U32 edits, 
U32 eDen){
  KMODEL *M = (KMODEL *) Calloc(1, sizeof(KMODEL));
  U64    prod = 1, *mult;
  U32    n;

  if(ctx > KMAX_HASH_CTX){
    fprintf(stderr, "Error: context size cannot be greater than %d\n", 
    KMAX_HASH_CTX);
    exit(1);
    }

  mult           = (U64 *) Calloc(ctx, sizeof(U64));
  M->nPModels    = (U64) pow(ALPHABET_SIZE, ctx);
  M->ctx         = ctx;
  M->alphaDen    = aDen;
  M->edits       = edits;
  M->pModelIdx   = 0;
  M->pModelIdxIR = M->nPModels - 1;
  M->ir          = ir  == 0 ? 0 : 1;
  M->ref         = ref == 0 ? 0 : 1;

  if(ctx >= KHASH_TABLE_BEGIN_CTX){
    M->mode     = KHASH_TABLE_MODE;
    M->maxCount = DEFAULT_MAX_COUNT >> 8;
    InitKHashTable(M, col);
    }
  else{
    M->mode     = KARRAY_MODE;
    M->maxCount = DEFAULT_MAX_COUNT;
    InitKArray(M);
    }

  for(n = 0 ; n < M->ctx ; ++n){
    mult[n] = prod;
    prod <<= 2;
    }

  M->multiplier = mult[M->ctx-1];

  if(edits != 0){
    M->SUBS.seq       = CreateCBuffer(BUFFER_SIZE, BGUARD);
    M->SUBS.in        = 0;
    M->SUBS.idx       = 0;
    M->SUBS.mask      = (uint8_t *) Calloc(BGUARD, sizeof(uint8_t));
    M->SUBS.threshold = edits;
    M->SUBS.eDen      = eDen;
    }

  Free(mult);
  return M;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

KMODEL *CreateKShadowModel(KMODEL *XP){
  KMODEL      *M = (KMODEL *) Calloc(1, sizeof(KMODEL));
  M->nPModels    = XP->nPModels;
  M->ctx         = XP->ctx;
  M->alphaDen    = XP->alphaDen;
  M->edits       = XP->edits;
  M->pModelIdx   = XP->pModelIdx;
  M->pModelIdxIR = XP->pModelIdxIR;
  M->ir          = XP->ir;
  M->ref         = XP->ref;
  M->mode        = XP->mode;
  M->maxCount    = XP->maxCount;
  M->multiplier  = XP->multiplier;

  if(M->edits != 0){
    M->SUBS.seq       = CreateCBuffer(BUFFER_SIZE, BGUARD);
    M->SUBS.in        = XP->SUBS.in;
    M->SUBS.idx       = XP->SUBS.idx;
    M->SUBS.mask      = (uint8_t *) Calloc(BGUARD, sizeof(uint8_t));
    M->SUBS.threshold = XP->SUBS.threshold;
    M->SUBS.eDen      = XP->SUBS.eDen;
    }

  return M;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetKModelIdx(KMODEL *M){
  M->pModelIdx   = 0;
  M->pModelIdxIR = M->nPModels - 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetKShadowModel(KMODEL *M){
  ResetKModelIdx(M);
  if(M->edits != 0){
    RemoveCBuffer(M->SUBS.seq);
    M->SUBS.seq = CreateCBuffer(BUFFER_SIZE, BGUARD);
    M->SUBS.in  = 0;
    M->SUBS.idx = 0;
    uint32_t n;
    for(n = 0 ; n < BGUARD ; ++n)
      M->SUBS.mask[n] = 0;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t BestKId(uint32_t *f, uint32_t sum){
  if(sum == 4) return -2; // ZERO COUNTERS

  uint32_t x, best = 0, max = f[0];
  for(x = 1 ; x < 4 ; ++x)
    if(f[x] > max){
      max = f[x];
      best = x;
      }

  for(x = 0 ; x < 4 ; ++x) if(best != x && max == f[x]) return -1;

  return best;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline U8 GetKPModelIdxIR(U8 *p, KMODEL *M){
  M->pModelIdxIR = (M->pModelIdxIR>>2)+GetCompNum(*p)*M->multiplier;
  return GetCompNum(*(p-M->ctx));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void GetKPModelIdx(U8 *p, KMODEL *M){
  M->pModelIdx = ((M->pModelIdx-*(p-M->ctx)*M->multiplier)<<2)+*p;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline uint64_t GetKPModelIdxCorr(U8 *p, KMODEL *M, uint64_t idx){
  return (((idx-*(p-M->ctx)*M->multiplier)<<2)+*p);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void FailKSUBS(KMODEL *M){
  uint32_t x, fails = 0;
  for(x = 0 ; x < M->ctx ; ++x)
    if(M->SUBS.mask[x] != 0)
      ++fails;
  if(fails <= M->SUBS.threshold)
    ShiftBuffer(M->SUBS.mask, M->ctx, 1);
  else 
    M->SUBS.in = 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void HitKSUBS(KMODEL *M){
  ShiftBuffer(M->SUBS.mask, M->ctx, 0);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CorrectKModelSUBS(KMODEL *M, PModel *P, uint8_t sym){
  int32_t best = BestKId(P->freqs, P->sum);
  switch(best){
    case -2:  // IT IS A ZERO COUNTER [NOT SEEN BEFORE]
      if(M->SUBS.in != 0)
        FailKSUBS(M);
    break;
    case -1:  // IT HAS AT LEAST TWO MAXIMUM FREQS [CONFUSION FREQS]
      if(M->SUBS.in != 0)
        HitKSUBS(M);
    break;
    default:  // IT HAS ONE MAXIMUM FREQ
      if(M->SUBS.in == 0){ // IF IS OUT
        M->SUBS.in = 1;
        memset(M->SUBS.mask, 0, M->ctx);
        }
      else{ // IF IS IN
        if(best == sym) HitKSUBS(M);
        else{
          FailKSUBS(M);
          M->SUBS.seq->buf[M->SUBS.seq->idx] = best; 
          } // UPDATE BUFFER WITH NEW SYMBOL
        }
    }
  UpdateCBuffer(M->SUBS.seq);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
void CorrectKXModels(KMODEL **Shadow, PModel **PM, uint8_t sym){
  uint32_t n = 0, m;
        
  for(m = 0 ; m < P->nModels ; ++m){
    if(Shadow[m]->edits != 0)
      CorrectKModelSUBS(Shadow[m], PM[++n], sym);
    ++n;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void ComputeKModel(KMODEL *M, PModel *P, uint64_t idx, uint32_t aDen){
  ACC *ac;
  switch(M->mode){
    case KHASH_TABLE_MODE:
      GetKHCCounters(&M->hTable, ZHASH(idx), P, aDen);
    break;
    case KARRAY_MODE:
      ac = &M->array.counters[idx<<2];
      P->freqs[0] = 1 + aDen * ac[0];
      P->freqs[1] = 1 + aDen * ac[1];
      P->freqs[2] = 1 + aDen * ac[2];
      P->freqs[3] = 1 + aDen * ac[3];
      P->sum = P->freqs[0] + P->freqs[1] + P->freqs[2] + P->freqs[3];
    break;
    default:
    fprintf(stderr, "Error: not implemented!\n");
    exit(1);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
