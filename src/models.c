#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "defs.h"
#include "mem.h"
#include "common.h"
#include "models.h"

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

static void InitHashTable(CModel *M, U32 c){ 
  uint32_t k;
  M->hTable.maxC    = c;
  M->hTable.index   = (ENTMAX *) Calloc(HASH_SIZE, sizeof(ENTMAX));
  M->hTable.entries = (Entry **) Calloc(HASH_SIZE, sizeof(Entry *));
  for(k = 0 ; k < HASH_SIZE ; ++k)
    M->hTable.entries[k] = (Entry *) Calloc(M->hTable.maxC, sizeof(Entry));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void FreeCModel(CModel *M){
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

void FreeShadow(CModel *M){
  if(M->edits != 0){
    Free(M->SUBS.mask);
    RemoveCBuffer(M->SUBS.seq);
    }
  Free(M);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CMWeight *CreateWeightModel(uint32_t size){
  uint32_t n;
  CMWeight *CMW    = (CMWeight *) Calloc(1, sizeof(CMWeight));
  CMW->totModels   = size;
  CMW->totalWeight = 0;
  CMW->weight      = (double *) Calloc(CMW->totModels, sizeof(double));
  for(n = 0 ; n < CMW->totModels ; ++n)
    CMW->weight[n] = 1.0 / CMW->totModels;
  return CMW;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetWeightModel(CMWeight *CMW){
  uint32_t n;
  double fraction = 1.0 / CMW->totModels;
  CMW->totalWeight = 0;
  for(n = 0 ; n < CMW->totModels ; ++n)
    CMW->weight[n] = fraction;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RenormalizeWeights(CMWeight *CMW){
  uint32_t n;
  for(n = 0 ; n < CMW->totModels ; ++n) 
    CMW->weight[n] /= CMW->totalWeight;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CalcDecayment(CMWeight *CMW, PModel **PM, uint8_t sym, double gamma){
  uint32_t n;
  CMW->totalWeight = 0;
  for(n = 0 ; n < CMW->totModels ; ++n){
    CMW->weight[n] = Power(CMW->weight[n], gamma) * (double) PM[n]->freqs[sym] 
    / PM[n]->sum;
    CMW->totalWeight += CMW->weight[n];
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteWeightModel(CMWeight *CMW){
  Free(CMW->weight);
  Free(CMW);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void InitArray(CModel *M){
  M->array.counters = (ACC *) Calloc(M->nPModels<<2, sizeof(ACC));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void InsertKey(HashTable *H, U32 hi, U64 idx, U8 s){
  if(++H->index[hi] == H->maxC)
    H->index[hi] = 0;

  #if defined(PREC32B)
  H->entries[hi][H->index[hi]].key = (U32)(idx&0xffffffff);
  #elif defined(PREC16B)
  H->entries[hi][H->index[hi]].key = (U16)(idx&0xffff);
  #else
  H->entries[hi][H->index[hi]].key = (U8)(idx&0xff);
  #endif  
  H->entries[hi][H->index[hi]].counters = (0x01<<(s<<2));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void GetFreqsFromHCC(HCC c, uint32_t a, PModel *P){
   P->sum  = (P->freqs[0] = a * ( c &  0x0f) + 1);
   P->sum += (P->freqs[1] = a * ((c & (0x0f<<4 ))>>4)  + 1);
   P->sum += (P->freqs[2] = a * ((c & (0x0f<<8 ))>>8)  + 1);
   P->sum += (P->freqs[3] = a * ((c & (0x0f<<12))>>12) + 1);
   }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void ComputeMXProbs(FloatPModel *PT, PModel *MX){
  MX->sum  = (MX->freqs[0] = 1 + (unsigned) (PT->freqs[0] * MX_PMODEL));
  MX->sum += (MX->freqs[1] = 1 + (unsigned) (PT->freqs[1] * MX_PMODEL));
  MX->sum += (MX->freqs[2] = 1 + (unsigned) (PT->freqs[2] * MX_PMODEL));
  MX->sum += (MX->freqs[3] = 1 + (unsigned) (PT->freqs[3] * MX_PMODEL)); 
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GetHCCounters(HashTable *H, U64 key, PModel *P, uint32_t a){
  U32 n, hIndex = key % HASH_SIZE;
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
      GetFreqsFromHCC(H->entries[hIndex][n].counters, a, P);
      return;
      }
    }
  // FROM MAX_COLISIONS TO INDEX
  for(n = (H->maxC-1) ; n > pos ; --n){
    if(H->entries[hIndex][n].key == b){
      GetFreqsFromHCC(H->entries[hIndex][n].counters, a, P);
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

PModel *CreatePModel(U32 n){
  PModel *PM = (PModel *) Calloc(1, sizeof(PModel));
  PM->freqs  = (U32    *) Calloc(n, sizeof(U32));
  return PM;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemovePModel(PModel *PM){
  Free(PM->freqs);
  Free(PM);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FloatPModel *CreateFloatPModel(U32 n){
  FloatPModel *F = (FloatPModel *) Calloc(1, sizeof(FloatPModel));
  F->freqs = (double *) Calloc(n, sizeof(double));
  return F;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveFPModel(FloatPModel *FM){
  Free(FM->freqs);
  Free(FM);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateCModelCounter(CModel *M, U32 sym, U64 im){
  U32 n;
  ACC *AC;
  U64 idx = im;

  if(M->mode == HASH_TABLE_MODE){
    U16 counter, sc;
    U32 s, hIndex = (idx = ZHASH(idx)) % HASH_SIZE;
    #if defined(PREC32B)
    U32 b = idx & 0xffffffff;
    #elif defined(PREC16B)
    U16 b = idx & 0xffff;
    #else
    U8  b = idx & 0xff;
    #endif

    for(n = 0 ; n < M->hTable.maxC ; ++n){
      if(M->hTable.entries[hIndex][n].key == b){
        sc = (M->hTable.entries[hIndex][n].counters>>(sym<<2))&0x0f;
        if(sc == 15){ // IT REACHES THE MAXIMUM COUNTER: RENORMALIZE
          for(s = 0 ; s < 4 ; ++s){ // RENORMALIZE EACH AND STORE
            counter = ((M->hTable.entries[hIndex][n].counters>>(s<<2))&0x0f)>>1;
            M->hTable.entries[hIndex][n].counters &= ~(0x0f<<(s<<2));
            M->hTable.entries[hIndex][n].counters |= (counter<<(s<<2));
            }
          }
        // GET, INCREMENT AND STORE COUNTER
        sc = (M->hTable.entries[hIndex][n].counters>>(sym<<2))&0x0f;
        ++sc;
        M->hTable.entries[hIndex][n].counters &= ~(0x0f<<(sym<<2));
        M->hTable.entries[hIndex][n].counters |= (sc<<(sym<<2));
        return;
        }
      }
    InsertKey(&M->hTable, hIndex, b, sym); // KEY NOT FOUND: WRITE ON OLDEST
    }
  else{
    AC = &M->array.counters[idx << 2];
    if(++AC[sym] == M->maxCount){    
      AC[0] >>= 1;
      AC[1] >>= 1;
      AC[2] >>= 1;
      AC[3] >>= 1;
      }
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CModel *CreateCModel(U32 ctx, U32 aDen, U32 ir, U8 ref, U32 col, U32 edits, 
U32 eDen){
  CModel *M = (CModel *) Calloc(1, sizeof(CModel));
  U64    prod = 1, *mult;
  U32    n;

  if(ctx > MAX_HASH_CTX){
    fprintf(stderr, "Error: context size cannot be greater than %d\n", 
    MAX_HASH_CTX);
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

  if(ctx >= HASH_TABLE_BEGIN_CTX){
    M->mode     = HASH_TABLE_MODE;
    M->maxCount = DEFAULT_MAX_COUNT >> 8;
    InitHashTable(M, col);
    }
  else{
    M->mode     = ARRAY_MODE;
    M->maxCount = DEFAULT_MAX_COUNT;
    InitArray(M);
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

CModel *CreateShadowModel(CModel *XP){
  CModel *M = (CModel *) Calloc(1, sizeof(CModel));
   
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

void ResetCModelIdx(CModel *M){
  M->pModelIdx   = 0;
  M->pModelIdxIR = M->nPModels - 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetShadowModel(CModel *M){
  ResetCModelIdx(M);
  if(M->edits != 0){
    RemoveCBuffer(M->SUBS.seq);
    M->SUBS.seq = CreateCBuffer(BUFFER_SIZE, BGUARD);
    M->SUBS.in  = 0;
    M->SUBS.idx = 0;
    uint32_t n;
    for(n = 0 ; n < BGUARD ; ++n)
      M->SUBS.mask[n] = 0;
    //TODO: MEMSET?
    // Free(M->SUBS.mask);
    // M->SUBS.mask = (uint8_t *) Calloc(BGUARD, sizeof(uint8_t));
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t BestId(uint32_t *f, uint32_t sum){
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

inline U8 GetPModelIdxIR(U8 *p, CModel *M){
  M->pModelIdxIR = (M->pModelIdxIR>>2)+GetCompNum(*p)*M->multiplier;
  return GetCompNum(*(p-M->ctx));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void GetPModelIdx(U8 *p, CModel *M){
  M->pModelIdx = ((M->pModelIdx-*(p-M->ctx)*M->multiplier)<<2)+*p;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline uint64_t GetPModelIdxCorr(U8 *p, CModel *M, uint64_t idx){
  return (((idx-*(p-M->ctx)*M->multiplier)<<2)+*p);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void FailSUBS(CModel *M){
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

void HitSUBS(CModel *M){
  ShiftBuffer(M->SUBS.mask, M->ctx, 0);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CorrectCModelSUBS(CModel *M, PModel *P, uint8_t sym){
  int32_t best = BestId(P->freqs, P->sum);
  switch(best){
    case -2:  // IT IS A ZERO COUNTER [NOT SEEN BEFORE]
      if(M->SUBS.in != 0)
        FailSUBS(M);
    break;
    case -1:  // IT HAS AT LEAST TWO MAXIMUM FREQS [CONFUSION FREQS]
      if(M->SUBS.in != 0)
        HitSUBS(M);
    break;
    default:  // IT HAS ONE MAXIMUM FREQ
      if(M->SUBS.in == 0){ // IF IS OUT
        M->SUBS.in = 1;
        memset(M->SUBS.mask, 0, M->ctx);
        }
      else{ // IF IS IN
        if(best == sym) HitSUBS(M);
        else{
          FailSUBS(M);
          M->SUBS.seq->buf[M->SUBS.seq->idx] = best; 
          } // UPDATE BUFFER WITH NEW SYMBOL
        }
    }
  UpdateCBuffer(M->SUBS.seq);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
void CorrectXModels(CModel **Shadow, PModel **PM, uint8_t sym){
  uint32_t n = 0, m;
        
  for(m = 0 ; m < P->nModels ; ++m){
    if(Shadow[m]->edits != 0)
      CorrectCModelSUBS(Shadow[m], PM[++n], sym);
    ++n;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void ComputePModel(CModel *M, PModel *P, uint64_t idx, uint32_t aDen){
  ACC *ac;
  switch(M->mode){
    case HASH_TABLE_MODE:
      GetHCCounters(&M->hTable, ZHASH(idx), P, aDen);
    break;
    case ARRAY_MODE:
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

inline void ComputeWeightedFreqs(double w, PModel *P, FloatPModel *PT){
  double f = w / P->sum;
  PT->freqs[0] += (double) P->freqs[0] * f;
  PT->freqs[1] += (double) P->freqs[1] * f;
  PT->freqs[2] += (double) P->freqs[2] * f;
  PT->freqs[3] += (double) P->freqs[3] * f;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double PModelSymbolLog(PModel *P, U32 s){
  // TODO: LOG_2 FAST ?
  return log((double) P->sum / P->freqs[s]) / M_LN2;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int SelfSimilarity(uint8_t *seq, uint64_t init, uint64_t end){
  uint64_t n, bases = 0;
  double   bits = 0;
  CBUF     *symBuf  = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t  *readBuf = Calloc(BUFFER_SIZE, sizeof(uint8_t));
  uint8_t  sym, irSym = 0;
  CModel   *CM = CreateCModel(13, 1, 1, 1, 0, 0, 0);
  PModel   *PM = CreatePModel(ALPHABET_SIZE);
  
  for(n = init-1 ; n < end ; ++n){
    symBuf->buf[symBuf->idx] = sym = seq[n];

    if(sym == 4){ // IF EXIST: SKIP OTHER CHARS
      bits += 2.0;
      ++bases;
      continue;
      }

    GetPModelIdx(symBuf->buf+symBuf->idx-1, CM);
    if(CM->ir == 1) // INVERTED REPEATS
      irSym = GetPModelIdxIR(symBuf->buf+symBuf->idx, CM);

    ComputePModel(CM, PM, CM->pModelIdx, CM->alphaDen);
    bits += PModelSymbolLog(PM, sym);

    if(bases >= CM->ctx){
      UpdateCModelCounter(CM, sym, CM->pModelIdx);
      if(CM->ir == 1) // INVERTED REPEATS
        UpdateCModelCounter(CM, irSym, CM->pModelIdxIR);
      }

    ++bases;
    }

  FreeCModel(CM);  
  RemovePModel(PM);
  RemoveCBuffer(symBuf);
  Free(readBuf);

  if(bits / 2 > bases)
    return 2;
  else if (bits > bases)
    return 1;
  else
    return 0;

  //return bits > bases ? 1 : 0; // 1-COMPLEX, 0-LOWCOMPLEX
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
