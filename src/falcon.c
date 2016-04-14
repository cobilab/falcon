#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include "mem.h"
#include "time.h"
#include "defs.h"
#include "param.h"
#include "msg.h"
#include "top.h"
#include "parser.h"
#include "buffer.h"
#include "levels.h"
#include "common.h"
#include "models.h"
#include "pmodels.h"
#include "kmodels.h"
#include "stream.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CModel **Models;   // MEMORY SHARED BY THREADING
KMODEL **KModels;  // MEMORY SHARED BY THREADING


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - R E S E T   M O D E L S - - - - - - - - - - - -

void ResetModelsAndParam(CBUF *Buf, CModel **Shadow, CMWeight *CMW){
  uint32_t n;
  ResetCBuffer(Buf);
  for(n = 0 ; n < P->nModels ; ++n)
    ResetShadowModel(Shadow[n]);
  ResetWeightModel(CMW);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - R E S E T   K M O D E L S - - - - - - - - - - - -

void ResetKModelsAndParam(CBUF *Buf, KMODEL **Shadow, CMWeight *CMW){
  uint32_t n;
  ResetCBuffer(Buf);
  for(n = 0 ; n < P->nModels ; ++n)
    ResetKShadowModel(Shadow[n]);
  ResetWeightModel(CMW);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - L O C A L   C O M P L E X I T Y - - - - - - - - - - - -

#ifdef LOCAL_SIMILARITY
void LocalComplexity(Threads T, TOP *Top, uint64_t topSize, FILE *OUT){
  FILE        *Reader = Fopen(P->base, "r");
  double      bits = 0, instant = 0;
  uint64_t    nBase = 0, entry;
  uint32_t    n, totModels, cModel;
  PARSER      *PA = CreateParser();
  CBUF        *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t     *readBuf = (uint8_t *) Calloc(BUFFER_SIZE, sizeof(uint8_t));
  uint8_t     *pos;
  PModel      **pModel, *MX;
  CModel      **Shadow; // SHADOWS FOR SUPPORTING MODELS WITH THREADING
  FloatPModel *PT;
  CMWeight    *CMW;
  int         sym;

  totModels = P->nModels; // EXTRA MODELS DERIVED FROM EDITS
  for(n = 0 ; n < P->nModels ; ++n) 
    if(T.model[n].edits != 0)
      totModels += 1;

  Shadow      = (CModel **) Calloc(P->nModels, sizeof(CModel *));
  for(n = 0 ; n < P->nModels ; ++n)
    Shadow[n] = CreateShadowModel(Models[n]); 
  pModel      = (PModel **) Calloc(totModels, sizeof(PModel *));
  for(n = 0 ; n < totModels ; ++n)
    pModel[n] = CreatePModel(ALPHABET_SIZE);
  MX          = CreatePModel(ALPHABET_SIZE);
  PT          = CreateFloatPModel(ALPHABET_SIZE);
  CMW         = CreateWeightModel(totModels);

  for(entry = 0 ; entry < topSize ; ++entry){
    if(Top->V[entry].size > 1){ 
      fprintf(stderr, "      [+] Running profile: %-5"PRIu64" ... ", entry + 1);

      // PRINT HEADER COMPLEXITY VALUE
      fprintf(OUT, "#\t%.5lf\t%"PRIu64"\t%s\n", (1.0-Top->V[entry].value)*100.0, 
      Top->V[entry].size, Top->V[entry].name);

      // MOVE POINTER FORWARD
      Fseeko(Reader, (off_t) Top->V[entry].iPos-1, SEEK_SET); 

      while((sym = fgetc(Reader)) != EOF){

        if(sym == '>'){ // FOUND HEADER & SKIP 
          while((sym = fgetc(Reader)) != '\n' && sym != EOF)
            ; // DO NOTHING

          if(sym == EOF) 
            break;     // END OF FILE: QUIT
          }

        if(nBase >= Top->V[entry].size) // IT PROCESSED ALL READ BASES: QUIT!
          break;

        if(sym == '\n') continue;  // SKIP '\n' IN FASTA

        if((sym = DNASymToNum(sym)) == 4){
          fprintf(OUT, "%c", PackByte(2.0, sym)); // PRINT COMPLEXITY & SYM IN1
          continue; // IT IGNORES EXTRA SYMBOLS
          }

        symBuf->buf[symBuf->idx] = sym;
        memset((void *)PT->freqs, 0, ALPHABET_SIZE * sizeof(double));
        n = 0;
        pos = &symBuf->buf[symBuf->idx-1];
        for(cModel = 0 ; cModel < P->nModels ; ++cModel){
          CModel *CM = Shadow[cModel];
          GetPModelIdx(pos, CM);
          ComputePModel(Models[cModel], pModel[n], CM->pModelIdx, CM->alphaDen);
          ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
          if(CM->edits != 0){
            ++n;
            CM->SUBS.seq->buf[CM->SUBS.seq->idx] = sym;
            CM->SUBS.idx = GetPModelIdxCorr(CM->SUBS.seq->buf+CM->SUBS.seq->idx
            -1, CM, CM->SUBS.idx);
            ComputePModel(Models[cModel], pModel[n], CM->SUBS.idx, CM->SUBS.eDen);
            ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
            }
          ++n;
          }

        ComputeMXProbs(PT, MX);
        instant = PModelSymbolLog(MX, sym);
        bits += instant;
        fprintf(OUT, "%c", PackByte(instant, sym)); // PRINT COMPLEX & SYM IN1
        ++nBase;
        CalcDecayment(CMW, pModel, sym, P->gamma);
        RenormalizeWeights(CMW);
        CorrectXModels(Shadow, pModel, sym);
        UpdateCBuffer(symBuf);
        }

      if(entry < topSize - 1){ // RESET MODELS & PROPERTIES
        ResetModelsAndParam(symBuf, Shadow, CMW);
        nBase = bits = 0;
        }

      fprintf(OUT, "\n");
      fprintf(stderr, "Done!\n");
      }
    } 

  DeleteWeightModel(CMW);
  for(n = 0 ; n < totModels ; ++n)
    RemovePModel(pModel[n]);
  Free(pModel);
  RemovePModel(MX);
  RemoveFPModel(PT);
  for(n = 0 ; n < P->nModels ; ++n)
    FreeShadow(Shadow[n]);
  Free(Shadow);
  Free(readBuf);
  RemoveCBuffer(symBuf);
  RemoveParser(PA);
  fclose(Reader);
  }
#endif


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - L O C A L   C O M P L E X I T Y   W   K M - - - - - - - -

#ifdef LOCAL_SIMILARITY
void LocalComplexityWKM(Threads T, TOP *Top, uint64_t topSize, FILE *OUT){
  FILE        *Reader = Fopen(P->base, "r");
  double      bits = 0, instant = 0;
  uint64_t    nBase = 0, entry;
  uint32_t    n, totModels, model;
  PARSER      *PA = CreateParser();
  CBUF        *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t     *readBuf = (uint8_t *) Calloc(BUFFER_SIZE, sizeof(uint8_t));
  PModel      **pModel, *MX;
  KMODEL      **Shadow; // SHADOWS FOR SUPPORTING MODELS WITH THREADING
  FloatPModel *PT;
  CMWeight    *CMW;
  int         sym;

  totModels = P->nModels; // EXTRA MODELS DERIVED FROM EDITS
  for(n = 0 ; n < P->nModels ; ++n) 
    if(T.model[n].edits != 0)
      totModels += 1;

  Shadow      = (KMODEL **) Calloc(P->nModels, sizeof(KMODEL *));
  for(n = 0 ; n < P->nModels ; ++n)
    Shadow[n] = CreateKShadowModel(KModels[n]); 
  pModel      = (PModel **) Calloc(totModels, sizeof(PModel *));
  for(n = 0 ; n < totModels ; ++n)
    pModel[n] = CreatePModel(ALPHABET_SIZE);
  MX          = CreatePModel(ALPHABET_SIZE);
  PT          = CreateFloatPModel(ALPHABET_SIZE);
  CMW         = CreateWeightModel(totModels);

  for(entry = 0 ; entry < topSize ; ++entry){
    if(Top->V[entry].size > 1){ 
      fprintf(stderr, "      [+] Running profile: %-5"PRIu64" ... ", entry + 1);

      // PRINT HEADER COMPLEXITY VALUE
      fprintf(OUT, "#\t%.5lf\t%"PRIu64"\t%s\n", (1.0-Top->V[entry].value)*100.0, 
      Top->V[entry].size, Top->V[entry].name);

      // MOVE POINTER FORWARD
      Fseeko(Reader, (off_t) Top->V[entry].iPos-1, SEEK_SET); 

      while((sym = fgetc(Reader)) != EOF){

        if(sym == '>'){ // FOUND HEADER & SKIP 
          while((sym = fgetc(Reader)) != '\n' && sym != EOF)
            ; // DO NOTHING

          if(sym == EOF) 
            break;     // END OF FILE: QUIT
          }

        if(nBase >= Top->V[entry].size) // IT PROCESSED ALL READ BASES: QUIT!
          break;

        if(sym == '\n') continue;  // SKIP '\n' IN FASTA

        if((sym = DNASymToNum(sym)) == 4){
          fprintf(OUT, "%c", PackByte(2.0, sym)); // PRINT COMPLEXITY & SYM IN1
          continue; // IT IGNORES EXTRA SYMBOLS
          }

        symBuf->buf[symBuf->idx] = sym;
        memset((void *)PT->freqs, 0, ALPHABET_SIZE * sizeof(double));
        n = 0;
        for(model = 0 ; model < P->nModels ; ++model){
          KMODEL *KM = Shadow[model];
          GetKIdx(symBuf->buf+symBuf->idx, KM);
          ComputeKPModel(KModels[model], pModel[n], KM->idx-sym, KM->alphaDen);
          ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
/*          if(KM->edits != 0){
            ++n;
            KM->SUBS.seq->buf[KM->SUBS.seq->idx] = sym;
            KM->SUBS.idx = GetPModelIdxCorr(KM->SUBS.seq->buf+KM->SUBS.seq->idx
            -1, KM, KM->SUBS.idx);
            ComputeKPModel(KModels[model], pModel[n], KM->SUBS.idx, KM->SUBS.eDen);
            ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
            }
*/
          ++n;
          }

        ComputeMXProbs(PT, MX);
        instant = PModelSymbolLog(MX, sym);
        bits += instant;
        fprintf(OUT, "%c", PackByte(instant, sym)); // PRINT COMPLEX & SYM IN1
        ++nBase;
        CalcDecayment(CMW, pModel, sym, P->gamma);
        RenormalizeWeights(CMW);
//        CorrectXModels(Shadow, pModel, sym);
        UpdateCBuffer(symBuf);
        }

      if(entry < topSize - 1){ // RESET MODELS & PROPERTIES
        ResetKModelsAndParam(symBuf, Shadow, CMW);
        nBase = bits = 0;
        }

      fprintf(OUT, "\n");
      fprintf(stderr, "Done!\n");
      }
    } 

  DeleteWeightModel(CMW);
  for(n = 0 ; n < totModels ; ++n)
    RemovePModel(pModel[n]);
  Free(pModel);
  RemovePModel(MX);
  RemoveFPModel(PT);
  for(n = 0 ; n < P->nModels ; ++n)
    FreeKShadow(Shadow[n]);
  Free(Shadow);
  Free(readBuf);
  RemoveCBuffer(symBuf);
  RemoveParser(PA);
  fclose(Reader);
  }
#endif


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - P E R E G R I N E   C O M P R E S S I O N - - - - - - - -

void SamplingCompressTarget(Threads T){
  FILE        *Reader  = Fopen(P->base, "r");
  double      bits = 0;
  uint64_t    nBase = 0, r = 0, idx = 0, initNSymbol, nSymbol;
  uint32_t    k, idxPos;
  PARSER      *PA = CreateParser();
  CBUF        *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t     *readBuf = (uint8_t *) Calloc(BUFFER_SIZE, sizeof(uint8_t));
  uint8_t     sym, *pos, conName[MAX_NAME];
  PModel      *pModel;
  CModel      *Shadow; // SHADOW FOR SUPPORTING MODEL WITH THREADING
  int         action;

  Shadow = CreateShadowModel(Models[0]);
  pModel = CreatePModel(ALPHABET_SIZE);

  initNSymbol = nSymbol = 0;
  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      if((action = ParseMF(PA, (sym = readBuf[idxPos]))) < 0){
        switch(action){
          case -1: // IT IS THE BEGGINING OF THE HEADER
            if(PA->nRead>1 && ((PA->nRead-1) % P->nThreads == T.id) && nBase>1){
              #ifdef LOCAL_SIMILARITY
              if(P->local == 1){
                UpdateTopWP(BPBB(bits, nBase), conName, T.top, nBase,
                initNSymbol, nSymbol);
                }
              else
                UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
              #else
              UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
              #endif
              }
            #ifdef LOCAL_SIMILARITY
            initNSymbol = nSymbol;
            #endif
            // RESET MODELS 
            ResetCBuffer(symBuf);
            ResetShadowModel(Shadow);
            idx = r = nBase = bits = 0;
          break;
          case -2: conName[r] = '\0'; break; // IT IS THE '\n' HEADER END
          case -3: // IF IS A SYMBOL OF THE HEADER
            if(r >= MAX_NAME-1)
              conName[r] = '\0';
            else{
              if(sym == ' ' || sym < 32 || sym > 126){ // PROTECT INTERVAL
                if(r == 0) continue;
                else       sym = '_'; // PROTECT OUT SYM WITH UNDERL
                }
              conName[r++] = sym;
              }
          break;
          case -99: break; // IF IS A SIMPLE FORMAT BREAK
          default: exit(1);
          }
        continue; // GO TO NEXT SYMBOL
        }

      if(PA->nRead % P->nThreads == T.id){
        if((sym = DNASymToNum(sym)) == 4) continue; // IT IGNORES EXTRA SYMBOLS
        symBuf->buf[symBuf->idx] = sym;
        pos = &symBuf->buf[symBuf->idx-1];
        GetPModelIdx(pos, Shadow);

        if(idx++ % P->sample == 0 && idx > Shadow->ctx){
          ComputePModel(Models[0], pModel, Shadow->pModelIdx, Shadow->alphaDen);
          bits += PModelSymbolLog(pModel, sym);
          ++nBase;
          }

        UpdateCBuffer(symBuf);
        }
      }

  if(PA->nRead % P->nThreads == T.id){
    #ifdef LOCAL_SIMILARITY
    if(P->local == 1)
      UpdateTopWP(BPBB(bits, nBase), conName, T.top, nBase,
      initNSymbol, nSymbol);
    else
      UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
    #else
    UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
    #endif
    }

  RemovePModel(pModel);
  FreeShadow(Shadow);
  Free(readBuf);
  RemoveCBuffer(symBuf);
  RemoveParser(PA);
  fclose(Reader);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - F A L C O N   C O M P R E S S I O N - - - - - - - -

void FalconCompressTarget(Threads T){
  FILE        *Reader  = Fopen(P->base, "r");
  double      bits = 0;
  uint64_t    nBase = 0, r = 0, idx = 0, nSymbol, initNSymbol;
  uint32_t    k, idxPos;
  PARSER      *PA = CreateParser();
  CBUF        *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t     *readBuf = (uint8_t *) Calloc(BUFFER_SIZE, sizeof(uint8_t));
  uint8_t     sym, *pos, conName[MAX_NAME];
  PModel      *pModel;
  CModel      *Shadow; // SHADOW FOR SUPPORTING MODEL WITH THREADING
  int         action;

  Shadow = CreateShadowModel(Models[0]);
  pModel = CreatePModel(ALPHABET_SIZE);

  nSymbol = initNSymbol = 0;
  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      if((action = ParseMF(PA, (sym = readBuf[idxPos]))) < 0){
        switch(action){
          case -1: // IT IS THE BEGGINING OF THE HEADER
            if(PA->nRead>1 && ((PA->nRead-1) % P->nThreads == T.id && nBase>1)){
              #ifdef LOCAL_SIMILARITY
              if(P->local == 1){
                UpdateTopWP(BPBB(bits, nBase), conName, T.top, nBase,
                initNSymbol, nSymbol);
                }
              else
                UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
              #else
              UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
              #endif
              }
            #ifdef LOCAL_SIMILARITY
            initNSymbol = nSymbol;
            #endif
            // RESET MODELS 
            ResetCBuffer(symBuf);
            ResetShadowModel(Shadow);
            idx   = 0;
            r     = 0;
            nBase = 0; 
            bits  = 0;
          break;
          case -2: conName[r] = '\0'; break; // IT IS THE '\n' HEADER END
          case -3: // IF IS A SYMBOL OF THE HEADER
            if(r >= MAX_NAME-1)
              conName[r] = '\0';
            else{
              if(sym == ' ' || sym < 32 || sym > 126){ // PROTECT INTERVAL
                if(r == 0) continue;
                else       sym = '_'; // PROTECT OUT SYM WITH UNDERL
                }
              conName[r++] = sym;
              }
          break;
          case -99: break; // IF IS A SIMPLE FORMAT BREAK
          default: exit(1);
          }
        continue; // GO TO NEXT SYMBOL
        }

      if(PA->nRead % P->nThreads == T.id){
        if((sym = DNASymToNum(sym)) == 4) continue; // IT IGNORES EXTRA SYMBOLS
        symBuf->buf[symBuf->idx] = sym;
        pos = &symBuf->buf[symBuf->idx-1];
        GetPModelIdx(pos, Shadow);
        if(++idx >= Shadow->ctx){
          ComputePModel(Models[0], pModel, Shadow->pModelIdx, Shadow->alphaDen);
          bits += PModelSymbolLog(pModel, sym);
          ++nBase;
          }
        UpdateCBuffer(symBuf);
        }
      }

  if(PA->nRead % P->nThreads == T.id){
    #ifdef LOCAL_SIMILARITY
    if(P->local == 1)
      UpdateTopWP(BPBB(bits, nBase), conName, T.top, nBase,
      initNSymbol, nSymbol);
    else
      UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
    #else
    UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
    #endif
    }

  RemovePModel(pModel);
  FreeShadow(Shadow);
  Free(readBuf);
  RemoveCBuffer(symBuf);
  RemoveParser(PA);
  fclose(Reader);
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - R I G H T - S I D E   C O M P R E S S I O N - - - - - - - -

void RightCompressTarget(Threads T){
  FILE        *Reader  = Fopen(P->base, "r");
  FILE        *Writter = Fopen("xtmpFalconRightProf.fal", "w");
  double      bits = 0, instant = 0;
  uint64_t    nBase = 0, nSymbol;
  uint32_t    n, k, idxPos, totModels, cModel;
  PARSER      *PA = CreateParser();
  CBUF        *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t     *readBuf = (uint8_t *) Calloc(BUFFER_SIZE, sizeof(uint8_t));
  uint8_t     sym, *pos;
  PModel      **pModel, *MX;
  CModel      **Shadow; // SHADOWS FOR SUPPORTING MODELS WITH THREADING
  FloatPModel *PT;
  CMWeight    *CMW;
  int         action;
  
  totModels = P->nModels; // EXTRA MODELS DERIVED FROM EDITS
  for(n = 0 ; n < P->nModels ; ++n) 
    if(T.model[n].edits != 0)
      totModels += 1;

  Shadow      = (CModel **) Calloc(P->nModels, sizeof(CModel *));
  for(n = 0 ; n < P->nModels ; ++n)
    Shadow[n] = CreateShadowModel(Models[n]); 
  pModel      = (PModel **) Calloc(totModels, sizeof(PModel *));
  for(n = 0 ; n < totModels ; ++n)
    pModel[n] = CreatePModel(ALPHABET_SIZE);
  MX          = CreatePModel(ALPHABET_SIZE);
  PT          = CreateFloatPModel(ALPHABET_SIZE);
  CMW         = CreateWeightModel(totModels);

  nSymbol = 0;
  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      ++nSymbol;
      if((action = ParseMF(PA, (sym = readBuf[idxPos]))) < 0){
        switch(action){
          case -1: // IT IS THE BEGGINING OF THE HEADER
            ResetModelsAndParam(symBuf, Shadow, CMW); // RESET MODELS
            nBase = bits = 0;
          break;
          case -2:  break; // IT IS THE '\n' HEADER END
          case -3:  break; // IF IS A SYMBOL OF THE HEADER
          case -99: break; // IF IS A SIMPLE FORMAT BREAK
          default: exit(1);
          }
        continue; // GO TO NEXT SYMBOL
        }

      if(PA->nRead % P->nThreads == T.id){

        if((sym = DNASymToNum(sym)) == 4){
          continue; // IT IGNORES EXTRA SYMBOLS
          }

        symBuf->buf[symBuf->idx] = sym;
        memset((void *)PT->freqs, 0, ALPHABET_SIZE * sizeof(double));
        n = 0;
        pos = &symBuf->buf[symBuf->idx-1];
        for(cModel = 0 ; cModel < P->nModels ; ++cModel){
          CModel *CM = Shadow[cModel];
          GetPModelIdx(pos, CM);
          ComputePModel(Models[cModel], pModel[n], CM->pModelIdx, CM->alphaDen);
          ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
          if(CM->edits != 0){
            ++n;
            CM->SUBS.seq->buf[CM->SUBS.seq->idx] = sym;
            CM->SUBS.idx = GetPModelIdxCorr(CM->SUBS.seq->buf+CM->SUBS.seq->idx
            -1, CM, CM->SUBS.idx);
            ComputePModel(Models[cModel], pModel[n], CM->SUBS.idx, CM->SUBS.eDen);
            ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
            }
          ++n;
          }

        ComputeMXProbs(PT, MX);
        bits += (instant = PModelSymbolLog(MX, sym));

        fprintf(Writter, "%f\n", (float) instant); //TODO: OBVIOUSLY IMPROVE

        ++nBase;
        CalcDecayment(CMW, pModel, sym, P->gamma);
        RenormalizeWeights(CMW);
        CorrectXModels(Shadow, pModel, sym);
        UpdateCBuffer(symBuf);
        }
      }

  DeleteWeightModel(CMW);
  for(n = 0 ; n < totModels ; ++n)
    RemovePModel(pModel[n]);
  Free(pModel);
  RemovePModel(MX);
  RemoveFPModel(PT);
  for(n = 0 ; n < P->nModels ; ++n)
    FreeShadow(Shadow[n]);
  Free(Shadow);
  Free(readBuf);
  RemoveCBuffer(symBuf);
  RemoveParser(PA);
  fclose(Reader);
  fclose(Writter);
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - L E F T - S I D E     C O M P R E S S I O N - - - - - - - -

void LeftCompressTarget(Threads T){
  FILE        *Reader  = Fopen(P->base, "r");
  FILE        *Writter = Fopen("xtmpFalconRightProf.fal", "w");
  double      bits = 0, instant = 0;
  uint64_t    nBase = 0, nSymbol;
  uint32_t    n, k, idxPos, totModels, cModel;
  PARSER      *PA = CreateParser();
  CBUF        *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t     *readBuf = (uint8_t *) Calloc(BUFFER_SIZE, sizeof(uint8_t));
  uint8_t     sym, *pos;
  PModel      **pModel, *MX;
  CModel      **Shadow; // SHADOWS FOR SUPPORTING MODELS WITH THREADING
  FloatPModel *PT;
  CMWeight    *CMW;
  int         action;
  
  totModels = P->nModels; // EXTRA MODELS DERIVED FROM EDITS
  for(n = 0 ; n < P->nModels ; ++n) 
    if(T.model[n].edits != 0)
      totModels += 1;

  Shadow      = (CModel **) Calloc(P->nModels, sizeof(CModel *));
  for(n = 0 ; n < P->nModels ; ++n)
    Shadow[n] = CreateShadowModel(Models[n]); 
  pModel      = (PModel **) Calloc(totModels, sizeof(PModel *));
  for(n = 0 ; n < totModels ; ++n)
    pModel[n] = CreatePModel(ALPHABET_SIZE);
  MX          = CreatePModel(ALPHABET_SIZE);
  PT          = CreateFloatPModel(ALPHABET_SIZE);
  CMW         = CreateWeightModel(totModels);

  nSymbol = 0;
  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      ++nSymbol;
      if((action = ParseMF(PA, (sym = readBuf[idxPos]))) < 0){
        switch(action){
          case -1: // IT IS THE BEGGINING OF THE HEADER
            ResetModelsAndParam(symBuf, Shadow, CMW); // RESET MODELS
            nBase = bits = 0;
          break;
          case -2:  break; // IT IS THE '\n' HEADER END
          case -3:  break; // IF IS A SYMBOL OF THE HEADER
          case -99: break; // IF IS A SIMPLE FORMAT BREAK
          default: exit(1);
          }
        continue; // GO TO NEXT SYMBOL
        }

      if(PA->nRead % P->nThreads == T.id){

        if((sym = DNASymToNum(sym)) == 4){
          continue; // IT IGNORES EXTRA SYMBOLS
          }

        symBuf->buf[symBuf->idx] = sym;
        memset((void *)PT->freqs, 0, ALPHABET_SIZE * sizeof(double));
        n = 0;
        pos = &symBuf->buf[symBuf->idx-1];
        for(cModel = 0 ; cModel < P->nModels ; ++cModel){
          CModel *CM = Shadow[cModel];
          GetPModelIdx(pos, CM);
          ComputePModel(Models[cModel], pModel[n], CM->pModelIdx, CM->alphaDen);
          ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
          if(CM->edits != 0){
            ++n;
            CM->SUBS.seq->buf[CM->SUBS.seq->idx] = sym;
            CM->SUBS.idx = GetPModelIdxCorr(CM->SUBS.seq->buf+CM->SUBS.seq->idx
            -1, CM, CM->SUBS.idx);
            ComputePModel(Models[cModel], pModel[n], CM->SUBS.idx, CM->SUBS.eDen);
            ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
            }
          ++n;
          }

        ComputeMXProbs(PT, MX);
        bits += (instant = PModelSymbolLog(MX, sym));

        fprintf(Writter, "%f\n", (float) instant); //TODO: OBVIOUSLY IMPROVE

        ++nBase;
        CalcDecayment(CMW, pModel, sym, P->gamma);
        RenormalizeWeights(CMW);
        CorrectXModels(Shadow, pModel, sym);
        UpdateCBuffer(symBuf);
        }
      }

  DeleteWeightModel(CMW);
  for(n = 0 ; n < totModels ; ++n)
    RemovePModel(pModel[n]);
  Free(pModel);
  RemovePModel(MX);
  RemoveFPModel(PT);
  for(n = 0 ; n < P->nModels ; ++n)
    FreeShadow(Shadow[n]);
  Free(Shadow);
  Free(readBuf);
  RemoveCBuffer(symBuf);
  RemoveParser(PA);
  fclose(Reader);
  fclose(Writter);
  }



//////////////////////////////////////////////////////////////////////////////
// - - - - - - - D O U B L E - S I D E   C O M P R E S S I O N - - - - - - - -

void DoubleCompressTarget(Threads T){
  FILE        *Reader = Fopen(P->base, "r");
  double      bits = 0, instant = 0;
  uint64_t    nBase = 0, r = 0, nSymbol, initNSymbol;
  uint32_t    n, k, idxPos, totModels, cModel;
  PARSER      *PA = CreateParser();
  CBUF        *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t     *readBuf = (uint8_t *) Calloc(BUFFER_SIZE, sizeof(uint8_t));
  uint8_t     sym, *pos, conName[MAX_NAME];
  PModel      **pModel, *MX;
  CModel      **Shadow; // SHADOWS FOR SUPPORTING MODELS WITH THREADING
  FloatPModel *PT;
  CMWeight    *CMW;
  int         action;
  STREAM      *AUX;

  totModels = P->nModels; // EXTRA MODELS DERIVED FROM EDITS
  for(n = 0 ; n < P->nModels ; ++n) 
    if(T.model[n].edits != 0)
      totModels += 1;

  Shadow      = (CModel **) Calloc(P->nModels, sizeof(CModel *));
  for(n = 0 ; n < P->nModels ; ++n)
    Shadow[n] = CreateShadowModel(Models[n]); 
  pModel      = (PModel **) Calloc(totModels, sizeof(PModel *));
  for(n = 0 ; n < totModels ; ++n)
    pModel[n] = CreatePModel(ALPHABET_SIZE);
  MX          = CreatePModel(ALPHABET_SIZE);
  PT          = CreateFloatPModel(ALPHABET_SIZE);
  CMW         = CreateWeightModel(totModels);
  AUX         = CreateStream(DEF_STREAM_SIZE);

  initNSymbol = nSymbol = 0;
  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      ++nSymbol;
      if((action = ParseMF(PA, (sym = readBuf[idxPos]))) < 0){
        switch(action){
          case -1: // IT IS THE BEGGINING OF THE HEADER

            ResetModelsAndParam(symBuf, Shadow, CMW);

            bits = 0;
            if((PA->nRead-1) % P->nThreads == T.id && PA->nRead>1 && nBase>1){
             
              for( ; AUX->idx > 0 ; AUX->idx--){  // COMPRESSING USING REVERSE DIRECTION

                if((sym = AUX->bases[AUX->idx]) == 4){
                  continue;
                  }
               
                symBuf->buf[symBuf->idx] = sym;
  
                memset((void *)PT->freqs, 0, ALPHABET_SIZE * sizeof(double));
                n = 0;
                pos = &symBuf->buf[symBuf->idx-1];
                for(cModel = 0 ; cModel < P->nModels ; ++cModel){
                  CModel *CM = Shadow[cModel];
                  GetPModelIdx(pos, CM);
                  ComputePModel(Models[cModel], pModel[n], CM->pModelIdx, CM->alphaDen);
                  ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
                  if(CM->edits != 0){
                    ++n;
                    CM->SUBS.seq->buf[CM->SUBS.seq->idx] = sym;
                    CM->SUBS.idx = GetPModelIdxCorr(CM->SUBS.seq->buf+CM->SUBS.seq->idx
                    -1, CM, CM->SUBS.idx);
                    ComputePModel(Models[cModel], pModel[n], CM->SUBS.idx, CM->SUBS.eDen);
                    ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
                    }
                  ++n;
                  }

                ComputeMXProbs(PT, MX);
      
                // MINIMUM CALC
                if((instant = PModelSymbolLog(MX, sym)) <= AUX->bits[AUX->idx])
                  bits += instant;
                else
                  bits += AUX->bits[AUX->idx];

                CalcDecayment(CMW, pModel, sym, P->gamma);
                RenormalizeWeights(CMW);
                CorrectXModels(Shadow, pModel, sym);
                UpdateCBuffer(symBuf);
                } 

              #ifdef LOCAL_SIMILARITY
              if(P->local == 1){
                UpdateTopWP(BPBB(bits, nBase), conName, T.top, nBase, 
                initNSymbol, nSymbol);
                }
              else
                UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
              #else
              UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
              #endif
              }
            #ifdef LOCAL_SIMILARITY
            initNSymbol = nSymbol; 
            #endif  
            ResetModelsAndParam(symBuf, Shadow, CMW); // RESET MODELS
            r = nBase = bits = 0;
          break;
          case -2: conName[r] = '\0'; break; // IT IS THE '\n' HEADER END
          case -3: // IF IS A SYMBOL OF THE HEADER
            if(r >= MAX_NAME-1)
              conName[r] = '\0';
            else{
              if(sym == ' ' || sym < 32 || sym > 126){ // PROTECT INTERVAL
                if(r == 0) continue;
                else       sym = '_'; // PROTECT OUT SYM WITH UNDERL
                }
              conName[r++] = sym;
              }
          break; 
          case -99: break; // IF IS A SIMPLE FORMAT BREAK
          default: exit(1);
          }
        continue; // GO TO NEXT SYMBOL
        }

      if(PA->nRead % P->nThreads == T.id){

        if((sym = DNASymToNum(sym)) == 4){
          //UpdateStream(AUX, 4, 2.0);
          continue; // IT IGNORES EXTRA SYMBOLS
          }

        symBuf->buf[symBuf->idx] = sym;
        memset((void *)PT->freqs, 0, ALPHABET_SIZE * sizeof(double));
        n = 0;
        pos = &symBuf->buf[symBuf->idx-1];
        for(cModel = 0 ; cModel < P->nModels ; ++cModel){
          CModel *CM = Shadow[cModel];
          GetPModelIdx(pos, CM);
          ComputePModel(Models[cModel], pModel[n], CM->pModelIdx, CM->alphaDen);
          ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
          if(CM->edits != 0){
            ++n;
            CM->SUBS.seq->buf[CM->SUBS.seq->idx] = sym;
            CM->SUBS.idx = GetPModelIdxCorr(CM->SUBS.seq->buf+CM->SUBS.seq->idx
            -1, CM, CM->SUBS.idx);
            ComputePModel(Models[cModel], pModel[n], CM->SUBS.idx, CM->SUBS.eDen);
            ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
            }
          ++n;
          }

        ComputeMXProbs(PT, MX);
        bits += (instant = PModelSymbolLog(MX, sym));
        UpdateStream(AUX, sym, instant);
        ++nBase;
        CalcDecayment(CMW, pModel, sym, P->gamma);
        RenormalizeWeights(CMW);
        CorrectXModels(Shadow, pModel, sym);
        UpdateCBuffer(symBuf);
        }
      }
        
  if(PA->nRead % P->nThreads == T.id){
    #ifdef LOCAL_SIMILARITY
    if(P->local == 1)
      UpdateTopWP(BPBB(bits, nBase), conName, T.top, nBase,
      initNSymbol, nSymbol);
    else
      UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
    #else
    UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
    #endif
    }

  RemoveStream(AUX);
  DeleteWeightModel(CMW);
  for(n = 0 ; n < totModels ; ++n)
    RemovePModel(pModel[n]);
  Free(pModel);
  RemovePModel(MX);
  RemoveFPModel(PT);
  for(n = 0 ; n < P->nModels ; ++n)
    FreeShadow(Shadow[n]);
  Free(Shadow);
  Free(readBuf);
  RemoveCBuffer(symBuf);
  RemoveParser(PA);
  fclose(Reader);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - C O M P R E S S I O N   W I T H   K M O D E L S - - - - - - - -

void CompressTargetWKM(Threads T){
  FILE        *Reader = Fopen(P->base, "r");
  double      bits = 0;
  uint64_t    nBase = 0, r = 0, nSymbol, initNSymbol;
  uint32_t    n, k, idxPos, totModels, model;
  PARSER      *PA = CreateParser();
  CBUF        *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t     *readBuf = (uint8_t *) Calloc(BUFFER_SIZE, sizeof(uint8_t));
  uint8_t     sym, conName[MAX_NAME];
  PModel      **pModel, *MX;
  KMODEL      **Shadow; // SHADOWS FOR SUPPORTING MODELS WITH THREADING
  FloatPModel *PT;
  CMWeight    *CMW;
  int         action;

  totModels = P->nModels; // EXTRA MODELS DERIVED FROM EDITS
  for(n = 0 ; n < P->nModels ; ++n) 
    if(T.model[n].edits != 0)
      totModels += 1;

  Shadow      = (KMODEL **) Calloc(P->nModels, sizeof(KMODEL *));
  for(n = 0 ; n < P->nModels ; ++n)
    Shadow[n] = CreateKShadowModel(KModels[n]); 
  pModel      = (PModel **) Calloc(totModels, sizeof(PModel *));
  for(n = 0 ; n < totModels ; ++n)
    pModel[n] = CreatePModel(ALPHABET_SIZE);
  MX          = CreatePModel(ALPHABET_SIZE);
  PT          = CreateFloatPModel(ALPHABET_SIZE);
  CMW         = CreateWeightModel(totModels);

  initNSymbol = nSymbol = 0;
  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      ++nSymbol;
      if((action = ParseMF(PA, (sym = readBuf[idxPos]))) < 0){
        switch(action){
          case -1: // IT IS THE BEGGINING OF THE HEADER
            if((PA->nRead-1) % P->nThreads == T.id && PA->nRead>1 && nBase>1){
              #ifdef LOCAL_SIMILARITY
              if(P->local == 1){
                UpdateTopWP(BPBB(bits, nBase), conName, T.top, nBase, 
                initNSymbol, nSymbol);
                }
              else
                UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
              #else
              UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
              #endif
              }
            #ifdef LOCAL_SIMILARITY
            initNSymbol = nSymbol; 
            #endif  
            ResetKModelsAndParam(symBuf, Shadow, CMW); // RESET MODELS
            r = nBase = bits = 0;
          break;
          case -2: conName[r] = '\0'; break; // IT IS THE '\n' HEADER END
          case -3: // IF IS A SYMBOL OF THE HEADER
            if(r >= MAX_NAME-1)
              conName[r] = '\0';
            else{
              if(sym == ' ' || sym < 32 || sym > 126){ // PROTECT INTERVAL
                if(r == 0) continue;
                else       sym = '_'; // PROTECT OUT SYM WITH UNDERL
                }
              conName[r++] = sym;
              }
          break; 
          case -99: break; // IF IS A SIMPLE FORMAT BREAK
          default: exit(1);
          }
        continue; // GO TO NEXT SYMBOL
        }

      if(PA->nRead % P->nThreads == T.id){

        if((sym = DNASymToNum(sym)) == 4){
          continue; // IT IGNORES EXTRA SYMBOLS
          }

        symBuf->buf[symBuf->idx] = sym;
        memset((void *)PT->freqs, 0, ALPHABET_SIZE * sizeof(double));
        n = 0;
        for(model = 0 ; model < P->nModels ; ++model){
          KMODEL *KM = Shadow[model];
          GetKIdx(symBuf->buf+symBuf->idx, KM);
          ComputeKPModel(KModels[model], pModel[n], KM->idx-sym, KM->alphaDen);
          ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
          /*
          if(KM->edits != 0){
            ++n;
            KM->SUBS.seq->buf[KM->SUBS.seq->idx] = sym;
            KM->SUBS.idx = GetPModelIdxCorr(KM->SUBS.seq->buf+KM->SUBS.seq->idx
            -1, KM, KM->SUBS.idx);
            ComputePModel(KModels[model], pModel[n], KM->SUBS.idx, KM->SUBS.eDen);
            ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
            }
          */
          ++n;
          }

        ComputeMXProbs(PT, MX);
        bits += PModelSymbolLog(MX, sym);
        ++nBase;
        CalcDecayment(CMW, pModel, sym, P->gamma);
        RenormalizeWeights(CMW);
        // CorrectXModels(Shadow, pModel, sym);
        UpdateCBuffer(symBuf);
        }
      }
        
  if(PA->nRead % P->nThreads == T.id){
    #ifdef LOCAL_SIMILARITY
    if(P->local == 1)
      UpdateTopWP(BPBB(bits, nBase), conName, T.top, nBase,
      initNSymbol, nSymbol);
    else
      UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
    #else
    UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
    #endif
    }

  DeleteWeightModel(CMW);
  for(n = 0 ; n < totModels ; ++n)
    RemovePModel(pModel[n]);
  Free(pModel);
  RemovePModel(MX);
  RemoveFPModel(PT);
  for(n = 0 ; n < P->nModels ; ++n)
    FreeKShadow(Shadow[n]);
  Free(Shadow);
  Free(readBuf);
  RemoveCBuffer(symBuf);
  RemoveParser(PA);
  fclose(Reader);
  }
  

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - C O M P R E S S I O N - - - - - - - - - - - - - 

void CompressTarget(Threads T){
  FILE        *Reader = Fopen(P->base, "r");
  double      bits = 0;
  uint64_t    nBase = 0, r = 0, nSymbol, initNSymbol;
  uint32_t    n, k, idxPos, totModels, cModel;
  PARSER      *PA = CreateParser();
  CBUF        *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t     *readBuf = (uint8_t *) Calloc(BUFFER_SIZE, sizeof(uint8_t));
  uint8_t     sym, *pos, conName[MAX_NAME];
  PModel      **pModel, *MX;
  CModel      **Shadow; // SHADOWS FOR SUPPORTING MODELS WITH THREADING
  FloatPModel *PT;
  CMWeight    *CMW;
  int         action;

  totModels = P->nModels; // EXTRA MODELS DERIVED FROM EDITS
  for(n = 0 ; n < P->nModels ; ++n) 
    if(T.model[n].edits != 0)
      totModels += 1;

  Shadow      = (CModel **) Calloc(P->nModels, sizeof(CModel *));
  for(n = 0 ; n < P->nModels ; ++n)
    Shadow[n] = CreateShadowModel(Models[n]); 
  pModel      = (PModel **) Calloc(totModels, sizeof(PModel *));
  for(n = 0 ; n < totModels ; ++n)
    pModel[n] = CreatePModel(ALPHABET_SIZE);
  MX          = CreatePModel(ALPHABET_SIZE);
  PT          = CreateFloatPModel(ALPHABET_SIZE);
  CMW         = CreateWeightModel(totModels);

  initNSymbol = nSymbol = 0;
  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      ++nSymbol;
      if((action = ParseMF(PA, (sym = readBuf[idxPos]))) < 0){
        switch(action){
          case -1: // IT IS THE BEGGINING OF THE HEADER
            if((PA->nRead-1) % P->nThreads == T.id && PA->nRead>1 && nBase>1){
              #ifdef LOCAL_SIMILARITY
              if(P->local == 1){
                UpdateTopWP(BPBB(bits, nBase), conName, T.top, nBase, 
                initNSymbol, nSymbol);
                }
              else
                UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
              #else
              UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
              #endif
              }
            #ifdef LOCAL_SIMILARITY
            initNSymbol = nSymbol; 
            #endif  
            ResetModelsAndParam(symBuf, Shadow, CMW); // RESET MODELS
            r = nBase = bits = 0;
          break;
          case -2: conName[r] = '\0'; break; // IT IS THE '\n' HEADER END
          case -3: // IF IS A SYMBOL OF THE HEADER
            if(r >= MAX_NAME-1)
              conName[r] = '\0';
            else{
              if(sym == ' ' || sym < 32 || sym > 126){ // PROTECT INTERVAL
                if(r == 0) continue;
                else       sym = '_'; // PROTECT OUT SYM WITH UNDERL
                }
              conName[r++] = sym;
              }
          break; 
          case -99: break; // IF IS A SIMPLE FORMAT BREAK
          default: exit(1);
          }
        continue; // GO TO NEXT SYMBOL
        }

      if(PA->nRead % P->nThreads == T.id){

        if((sym = DNASymToNum(sym)) == 4){
          continue; // IT IGNORES EXTRA SYMBOLS
          }

        symBuf->buf[symBuf->idx] = sym;
        memset((void *)PT->freqs, 0, ALPHABET_SIZE * sizeof(double));
        n = 0;
        pos = &symBuf->buf[symBuf->idx-1];
        for(cModel = 0 ; cModel < P->nModels ; ++cModel){
          CModel *CM = Shadow[cModel];
          GetPModelIdx(pos, CM);
          ComputePModel(Models[cModel], pModel[n], CM->pModelIdx, CM->alphaDen);
          ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
          if(CM->edits != 0){
            ++n;
            CM->SUBS.seq->buf[CM->SUBS.seq->idx] = sym;
            CM->SUBS.idx = GetPModelIdxCorr(CM->SUBS.seq->buf+CM->SUBS.seq->idx
            -1, CM, CM->SUBS.idx);
            ComputePModel(Models[cModel], pModel[n], CM->SUBS.idx, CM->SUBS.eDen);
            ComputeWeightedFreqs(CMW->weight[n], pModel[n], PT);
            }
          ++n;
          }

        ComputeMXProbs(PT, MX);
        bits += PModelSymbolLog(MX, sym);
        ++nBase;
        CalcDecayment(CMW, pModel, sym, P->gamma);
        RenormalizeWeights(CMW);
        CorrectXModels(Shadow, pModel, sym);
        UpdateCBuffer(symBuf);
        }
      }
        
  if(PA->nRead % P->nThreads == T.id){
    #ifdef LOCAL_SIMILARITY
    if(P->local == 1)
      UpdateTopWP(BPBB(bits, nBase), conName, T.top, nBase,
      initNSymbol, nSymbol);
    else
      UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
    #else
    UpdateTop(BPBB(bits, nBase), conName, T.top, nBase);
    #endif
    }

  DeleteWeightModel(CMW);
  for(n = 0 ; n < totModels ; ++n)
    RemovePModel(pModel[n]);
  Free(pModel);
  RemovePModel(MX);
  RemoveFPModel(PT);
  for(n = 0 ; n < P->nModels ; ++n)
    FreeShadow(Shadow[n]);
  Free(Shadow);
  Free(readBuf);
  RemoveCBuffer(symBuf);
  RemoveParser(PA);
  fclose(Reader);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - F   T H R E A D I N G - - - - - - - - - - - - - - -

void *CompressThread(void *Thr){
  Threads *T = (Threads *) Thr;
  
  //  if(P->nModels == 1 && T->model[0].edits == 0){
  //    if(P->sample > 1){
  //      SamplingCompressTarget(T[0]);
  //      }
  //    else{      
  //      FalconCompressTarget(T[0]);
  //      }
  //    }

  #ifdef KMODELSUSAGE
  CompressTargetWKM(T[0]);
  #else
    #ifdef DOUBLESIDE
    fprintf(stderr, "  [+] Running right side ......... ");
    RightCompressTarget(T[0]);
    fprintf(stderr, "Done!\n");

//    FreeReference();

//    LoadReference();
    
    LeftCompressTarget(T[0]);
    

    #else
    CompressTarget(T[0]);
    #endif
  #endif

  pthread_exit(NULL);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - R   T H R E A D I N G - - - - - - - - - - - - - - -

void *CompressThreadRight(void *Thr){
  Threads *T = (Threads *) Thr;
  RightCompressTarget(T[0]);
  pthread_exit(NULL);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - L   T H R E A D I N G - - - - - - - - - - - - - - -

void *CompressThreadLeft(void *Thr){
  Threads *T = (Threads *) Thr;
  LeftCompressTarget(T[0]);
  pthread_exit(NULL);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - R E F E R E N C E - - - - - - - - - - - - -

void LoadReference(char *refName){
  FILE     *Reader = Fopen(refName, "r");
  uint32_t n;
  uint64_t idx = 0;
  uint64_t k, idxPos;
  PARSER   *PA = CreateParser();
  CBUF     *symBuf  = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t  *readBuf = Calloc(BUFFER_SIZE, sizeof(uint8_t));
  uint8_t  sym, irSym = 0;
  FileType(PA, Reader);
  rewind(Reader);

  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      if(ParseSym(PA, (sym = readBuf[idxPos])) == -1){ idx = 0; continue; }
      symBuf->buf[symBuf->idx] = sym = DNASymToNum(sym);
      for(n = 0 ; n < P->nModels ; ++n){
        CModel *CM = Models[n];
        GetPModelIdx(symBuf->buf+symBuf->idx-1, CM);
        if(CM->ir == 1) // INVERTED REPEATS
          irSym = GetPModelIdxIR(symBuf->buf+symBuf->idx, CM);
        if(++idx >= CM->ctx){
          UpdateCModelCounter(CM, sym, CM->pModelIdx);
          if(CM->ir == 1) // INVERTED REPEATS
            UpdateCModelCounter(CM, irSym, CM->pModelIdxIR);
          }
        }
      UpdateCBuffer(symBuf);
      }
 
  for(n = 0 ; n < P->nModels ; ++n)
    ResetCModelIdx(Models[n]);
  RemoveCBuffer(symBuf);
  Free(readBuf);
  RemoveParser(PA);
  fclose(Reader);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - R E F E R E N C E   W I T H   K M O D E L S - - - - - - - -

void LoadReferenceWKM(char *refName){
  FILE     *Reader = Fopen(refName, "r");
  uint32_t n;
  uint64_t idx = 0, k, idxPos;
  PARSER   *PA = CreateParser();
  CBUF     *symBuf  = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t  *readBuf = Calloc(BUFFER_SIZE, sizeof(uint8_t)), sym;
  FileType(PA, Reader);
  rewind(Reader);

  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      if(ParseSym(PA, (sym = readBuf[idxPos])) == -1){ idx = 0; continue; }
      symBuf->buf[symBuf->idx] = sym = DNASymToNum(sym);
      for(n = 0 ; n < P->nModels ; ++n){
        KMODEL *KM = KModels[n];
        GetKIdx(symBuf->buf+symBuf->idx, KM);
        if(++idx >= KM->ctx)
          UpdateKModelCounter(KM, sym, KM->idx);
        }
      UpdateCBuffer(symBuf);
      }
 
  for(n = 0 ; n < P->nModels ; ++n)
    ResetKModelIdx(KModels[n]);
  RemoveCBuffer(symBuf);
  Free(readBuf);
  RemoveParser(PA);
  fclose(Reader);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - R E F E R E N C E   W I T H   K M O D E L S   I N   R E V - - - -

void LoadRevReferenceWKM(char *refName){
  FILE     *Reader = Fopen(refName, "r");
  uint32_t n;
  uint64_t idx = 0, k, idxPos;
  PARSER   *PA = CreateParser();
  CBUF     *symBuf  = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t  *readBuf = Calloc(BUFFER_SIZE, sizeof(uint8_t)), sym;
  FileType(PA, Reader);
  rewind(Reader);

  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      if(ParseSym(PA, (sym = readBuf[idxPos])) == -1){ idx = 0; continue; }
      symBuf->buf[symBuf->idx] = sym = DNASymToNum(sym);
      for(n = 0 ; n < P->nModels ; ++n){
        KMODEL *KM = KModels[n];
        GetKIdx(symBuf->buf+symBuf->idx, KM);
        if(++idx >= KM->ctx)
          UpdateKModelCounter(KM, sym, KM->idx);
        }
      UpdateCBuffer(symBuf);
      }

  for(n = 0 ; n < P->nModels ; ++n)
    ResetKModelIdx(KModels[n]);
  RemoveCBuffer(symBuf);
  Free(readBuf);
  RemoveParser(PA);
  fclose(Reader);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - R E F E R E N C E   I N   R E V - - - - - - - - - -

void LoadRevReference(char *refName){
  FILE     *Reader = Fopen(refName, "r");
  uint32_t n;
  uint64_t idx = 0, k, idxPos;
  PARSER   *PA = CreateParser();
  CBUF     *symBuf  = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t  *readBuf = Calloc(BUFFER_SIZE, sizeof(uint8_t)), sym;
  FileType(PA, Reader);
  rewind(Reader);

  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      if(ParseSym(PA, (sym = readBuf[idxPos])) == -1){ idx = 0; continue; }
      symBuf->buf[symBuf->idx] = sym = DNASymToNum(sym);
      for(n = 0 ; n < P->nModels ; ++n){
        KMODEL *KM = KModels[n];
        GetKIdx(symBuf->buf+symBuf->idx, KM);
        if(++idx >= KM->ctx)
          UpdateKModelCounter(KM, sym, KM->idx);
        }
      UpdateCBuffer(symBuf);
      }

  for(n = 0 ; n < P->nModels ; ++n)
    ResetKModelIdx(KModels[n]);
  RemoveCBuffer(symBuf);
  Free(readBuf);
  RemoveParser(PA);
  fclose(Reader);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - C O M P R E S S O R   M A I N - - - - - - - - - - - -

void CompressAction(Threads *T, char *refName, char *baseName){
  pthread_t t[P->nThreads];
  uint32_t n;
 
  #ifdef KMODELSUSAGE
  KModels = (KMODEL **) Malloc(P->nModels * sizeof(KMODEL *));
  for(n = 0 ; n < P->nModels ; ++n)
    KModels[n] = CreateKModel(T[0].model[n].ctx, T[0].model[n].den,
    T[0].model[n].ir, REFERENCE, P->col, T[0].model[n].edits,
    T[0].model[n].eDen);
  fprintf(stderr, "  [+] Loading metagenomic file ..... ");
  LoadReferenceWKM(refName);
  fprintf(stderr, "Done!\n");
  #else
  Models = (CModel **) Malloc(P->nModels * sizeof(CModel *));
  for(n = 0 ; n < P->nModels ; ++n)
    Models[n] = CreateCModel(T[0].model[n].ctx, T[0].model[n].den, 
    T[0].model[n].ir, REFERENCE, P->col, T[0].model[n].edits, 
    T[0].model[n].eDen);
  fprintf(stderr, "  [+] Loading metagenomic file ..... ");
  LoadReference(refName);
  fprintf(stderr, "Done!\n");
  #endif

  #ifdef DOUBLESIDE
  fprintf(stderr, "  [+] Running Right side ........... ");
  for(n = 0 ; n < P->nThreads ; ++n)
    pthread_create(&(t[n+1]), NULL, CompressThreadRight, (void *) &(T[n]));
  for(n = 0 ; n < P->nThreads ; ++n) // DO NOT JOIN FORS!
    pthread_join(t[n+1], NULL);
  fprintf(stderr, "Done!\n");

  fprintf(stderr, "  [+] Freeing compression models ... ");
  for(n = 0 ; n < P->nModels ; ++n)
    #ifdef KMODELSUSAGE
    FreeKModel(KModels[n]);
    #else
    FreeCModel(Models[n]);
    #endif
  #ifdef KMODELSUSAGE
  Free(KModels);
  #else
  Free(Models);
  #endif
  fprintf(stderr, "Done!\n");

    #ifdef KMODELSUSAGE
    KModels = (KMODEL **) Malloc(P->nModels * sizeof(KMODEL *));
    for(n = 0 ; n < P->nModels ; ++n)
      KModels[n] = CreateKModel(T[0].model[n].ctx, T[0].model[n].den,
      T[0].model[n].ir, REFERENCE, P->col, T[0].model[n].edits,
      T[0].model[n].eDen);
    fprintf(stderr, "  [+] Loading reverse file ......... ");
    LoadRevReferenceWKM(refName);
    fprintf(stderr, "Done!\n");
    #else
    Models = (CModel **) Malloc(P->nModels * sizeof(CModel *));
    for(n = 0 ; n < P->nModels ; ++n)
      Models[n] = CreateCModel(T[0].model[n].ctx, T[0].model[n].den,
      T[0].model[n].ir, REFERENCE, P->col, T[0].model[n].edits,
      T[0].model[n].eDen);
    fprintf(stderr, "  [+] Loading reverse file ......... ");
    LoadRevReference(refName);
    fprintf(stderr, "Done!\n");
    #endif

  fprintf(stderr, "  [+] Running Left side ............ ");
  for(n = 0 ; n < P->nThreads ; ++n)
    pthread_create(&(t[n+1]), NULL, CompressThreadLeft, (void *) &(T[n]));
  for(n = 0 ; n < P->nThreads ; ++n) // DO NOT JOIN FORS!
    pthread_join(t[n+1], NULL);
  fprintf(stderr, "Done!\n");
  #else  
  fprintf(stderr, "  [+] Compressing database ......... ");
  for(n = 0 ; n < P->nThreads ; ++n)
    pthread_create(&(t[n+1]), NULL, CompressThread, (void *) &(T[n]));
  for(n = 0 ; n < P->nThreads ; ++n) // DO NOT JOIN FORS!
    pthread_join(t[n+1], NULL);
  fprintf(stderr, "Done!\n");
  #endif
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - M A I N - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t main(int argc, char *argv[]){
  char     **p = *&argv, **xargv, *xpl = NULL;
  int32_t  xargc = 0;
  uint32_t n, k, col, ref, topSize;
  double   gamma;
  Threads  *T;
  
  P = (Parameters *) Malloc(1 * sizeof(Parameters));
  if((P->help = ArgsState(DEFAULT_HELP, p, argc, "-h")) == 1 || argc < 2){
    PrintMenu();
    return EXIT_SUCCESS;
    }

  if(ArgsState(DEF_VERSION, p, argc, "-V")){
    PrintVersion();
    return EXIT_SUCCESS;
    }

  if(ArgsState(0, p, argc, "-s")){
    PrintLevels(); 
    return EXIT_SUCCESS;
    }

  P->verbose  = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");
  P->force    = ArgsState  (DEFAULT_FORCE,   p, argc, "-F");
  #ifdef LOCAL_SIMILARITY
  P->local    = ArgsState  (DEFAULT_LOCAL,   p, argc, "-Z");
  #endif
  P->sample   = ArgsNum    (DEFAULT_SAMPLE,  p, argc, "-p", MIN_SAP, MAX_SAP);
  P->level    = ArgsNum    (0,               p, argc, "-l", MIN_LEV, MAX_LEV);
  topSize     = ArgsNum    (DEF_TOP,         p, argc, "-t", MIN_TOP, MAX_TOP);
  P->nThreads = ArgsNum    (DEFAULT_THREADS, p, argc, "-n", MIN_THREADS, 
  MAX_THREADS);

  P->nModels = 0;
  for(n = 1 ; n < argc ; ++n)
    if(strcmp(argv[n], "-m") == 0)
      P->nModels += 1;

  if(P->nModels == 0 && P->level == 0)
    P->level = DEFAULT_LEVEL;

  if(P->level != 0){
    xpl = GetLevels(P->level);
    xargc = StrToArgv(xpl, &xargv);
    for(n = 1 ; n < xargc ; ++n)
      if(strcmp(xargv[n], "-m") == 0)
        P->nModels += 1;
    }

  gamma = DEFAULT_GAMMA;
  for(n = 1 ; n < xargc ; ++n) 
    if(strcmp(xargv[n], "-g") == 0) 
      gamma = atof(xargv[n+1]);

  col = MAX_COLLISIONS;
  for(n = 1 ; n < xargc ; ++n) 
    if(strcmp(xargv[n], "-c") == 0) 
      col = atoi(xargv[n+1]);

  P->col       = ArgsNum    (col,   p, argc, "-c", 1, 253);
  P->gamma     = ArgsDouble (gamma, p, argc, "-g");
  P->gamma     = ((int)(P->gamma * 65536)) / 65536.0;
  P->output    = ArgsFileGen(p, argc, "-x", "top", ".csv");
  #ifdef LOCAL_SIMILARITY
  if(P->local == 1){
    P->outLoc  = ArgsFileGen(p, argc, "-y", "local", ".fal");
    }
  #endif

  FILE *OUTPUT = NULL;
  if(!P->force) 
    FAccessWPerm(P->output);
  OUTPUT = Fopen(P->output, "w");

  #ifdef LOCAL_SIMILARITY
  FILE *OUTLOC = NULL;
  if(P->local == 1){
    if(!P->force)
      FAccessWPerm(P->outLoc);
    OUTLOC = Fopen(P->outLoc, "w");
    }
  #endif 

  if(P->nModels == 0){
    fprintf(stderr, "Error: at least you need to use a context model!\n");
    return EXIT_FAILURE;
    }

  // READ MODEL PARAMETERS FROM XARGS & ARGS
  T = (Threads *) Calloc(P->nThreads, sizeof(Threads));
  for(ref = 0 ; ref < P->nThreads ; ++ref){
    T[ref].model = (ModelPar *) Calloc(P->nModels, sizeof(ModelPar));
    T[ref].id    = ref;
    T[ref].top   = CreateTop(topSize);
    k = 0;
    for(n = 1 ; n < argc ; ++n)
      if(strcmp(argv[n], "-m") == 0)
        T[ref].model[k++] = ArgsUniqModel(argv[n+1], 0);
    if(P->level != 0){
      for(n = 1 ; n < xargc ; ++n)
        if(strcmp(xargv[n], "-m") == 0)
          T[ref].model[k++] = ArgsUniqModel(xargv[n+1], 0);
      }
    }

  fprintf(stderr, "\n");
  if(P->verbose) PrintArgs(P, T[0], argv[argc-2], argv[argc-1], topSize);

  fprintf(stderr, "==[ PROCESSING ]====================\n");
  TIME *Time = CreateClock(clock());
  P->base = argv[argc-1];
  CompressAction(T, argv[argc-2], P->base);

  k = 0;
  P->top = CreateTop(topSize * P->nThreads);
  for(ref = 0 ; ref < P->nThreads ; ++ref){
    for(n = 0 ; n < T[ref].top->size-1 ; ++n){
      P->top->V[k].value = T[ref].top->V[n].value; 
      P->top->V[k].size  = T[ref].top->V[n].size; 
      #ifdef LOCAL_SIMILARITY
      if(P->local == 1){
        P->top->V[k].iPos = T[ref].top->V[n].iPos; 
        P->top->V[k].ePos = T[ref].top->V[n].ePos; 
        }
      #endif
      CopyStringPart(P->top->V[k].name, T[ref].top->V[n].name);
      ++k;
      }
    }
  
  fprintf(stderr, "  [+] Sorting top .................. ");
  qsort(P->top->V, k, sizeof(VT), SortByValue);
  fprintf(stderr, "Done!\n");

  fprintf(stderr, "  [+] Printing to output file ...... ");
  #ifdef LOCAL_SIMILARITY
  if(P->local == 1)
    PrintTopWP(OUTPUT, P->top, topSize);
  else
    PrintTop(OUTPUT, P->top, topSize);
  #else
  PrintTop(OUTPUT, P->top, topSize);
  #endif
  fclose(OUTPUT);
  fprintf(stderr, "Done!\n");

  #ifdef LOCAL_SIMILARITY
  if(P->local == 1){
    fprintf(stderr, "  [+] Running local similarity:\n");
    #ifdef KMODELSUSAGE
    LocalComplexityWKM(T[0], P->top, topSize, OUTLOC);
    #else
    LocalComplexity(T[0], P->top, topSize, OUTLOC);
    #endif
    fclose(OUTLOC);
    }
  #endif

  fprintf(stderr, "  [+] Freeing compression models ... ");
  for(n = 0 ; n < P->nModels ; ++n)
    #ifdef KMODELSUSAGE
    FreeKModel(KModels[n]);
    #else
    FreeCModel(Models[n]);
    #endif
  #ifdef KMODELSUSAGE
  Free(KModels);
  #else
  Free(Models);
  #endif
  fprintf(stderr, "Done!\n");

  StopTimeNDRM(Time, clock());
  fprintf(stderr, "\n");

  fprintf(stderr, "==[ RESULTS ]=======================\n");
  if(topSize <= 100){
    #ifdef LOCAL_SIMILARITY
    if(P->local == 1)
      PrintTopInfoWP(P->top, topSize);
    else
      PrintTopInfo(P->top, topSize);
    #else
    PrintTopInfo(P->top, topSize);
    #endif
    }
  else{
    fprintf(stderr, "Top results have been sent to file.\n");
    }
  fprintf(stderr, "\n");

  fprintf(stderr, "==[ STATISTICS ]====================\n");
  StopCalcAll(Time, clock());
  fprintf(stderr, "\n");

  RemoveClock(Time);
  DeleteTop(P->top);
  for(ref = 0 ; ref < P->nThreads ; ++ref){
    DeleteTop(T[ref].top);
    Free(T[ref].model);
    }
  Free(T);

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
