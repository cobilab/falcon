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

CModel **Models;  // MEMORY SHARED BY THREADING

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - P E R E G R I N E   C O M P R E S S I O N - - - - - - - -

void SamplingCompressTarget(Threads T){
  FILE        *Reader  = Fopen(P->base, "r");
  double      bits = 0;
  uint64_t    nBase = 0, r = 0, idx = 0;
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

  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      if((action = ParseMF(PA, (sym = readBuf[idxPos]))) < 0){
        switch(action){
          case -1: // IT IS THE BEGGINING OF THE HEADER
            if(PA->nRead > 1 && ((PA->nRead-1) % P->nThreads == T.id)){
              UpdateTop(BoundDouble(0.0, bits/2.0/nBase, 1.0), conName, T.top, 
              nBase);
              }
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

  if(PA->nRead % P->nThreads == T.id)
    UpdateTop(BoundDouble(0.0, bits/2/nBase, 1.0), conName, T.top, nBase);

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
  uint64_t    nBase = 0, r = 0, idx = 0;
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

  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      if((action = ParseMF(PA, (sym = readBuf[idxPos]))) < 0){
        switch(action){
          case -1: // IT IS THE BEGGINING OF THE HEADER
            if(PA->nRead > 1 && ((PA->nRead-1) % P->nThreads == T.id)){
              UpdateTop(BoundDouble(0.0, bits/2.0/nBase, 1.0), conName, T.top, 
              nBase);
              }
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
        if(++idx > Shadow->ctx){
          ComputePModel(Models[0], pModel, Shadow->pModelIdx, Shadow->alphaDen);
          bits += PModelSymbolLog(pModel, sym);
          ++nBase;
          UpdateCBuffer(symBuf);
          }
        }
      }

  if(PA->nRead % P->nThreads == T.id)
    UpdateTop(BoundDouble(0.0, bits/2/nBase, 1.0), conName, T.top, nBase);

  RemovePModel(pModel);
  FreeShadow(Shadow);
  Free(readBuf);
  RemoveCBuffer(symBuf);
  RemoveParser(PA);
  fclose(Reader);
  }
  

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - C O M P R E S S I N G - - - - - - - - - - - - - 

void CompressTarget(Threads T){
  FILE        *Reader  = Fopen(P->base, "r");
  double      bits = 0;
  uint64_t    nBase = 0, r = 0;
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

  while((k = fread(readBuf, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){
      if((action = ParseMF(PA, (sym = readBuf[idxPos]))) < 0){
        switch(action){
          case -1: // IT IS THE BEGGINING OF THE HEADER
            if(PA->nRead > 1 && ((PA->nRead-1) % P->nThreads == T.id)){
              UpdateTop(BoundDouble(0.0, bits/2.0/nBase, 1.0), conName, T.top, 
              nBase);
              }
            // RESET MODELS 
            ResetCBuffer(symBuf);
            for(n = 0 ; n < P->nModels ; ++n)
              ResetShadowModel(Shadow[n]);
            ResetWeightModel(CMW);
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

        if((sym = DNASymToNum(sym)) == 4) continue; // IT IGNORES EXTRA SYMBOLS
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
        
  if(PA->nRead % P->nThreads == T.id)
    UpdateTop(BoundDouble(0.0, bits/2/nBase, 1.0), conName, T.top, nBase);

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

  if(P->nModels == 1 && T->model[0].edits == 0){
    if(P->sample > 1){
      SamplingCompressTarget(T[0]);
      }
    else{      
      FalconCompressTarget(T[0]);
      }
    }
  else{
    CompressTarget(T[0]);
    }

  pthread_exit(NULL);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - R E F E R E N C E - - - - - - - - - - - - -

void LoadReference(char *refName){
  FILE     *Reader = Fopen(refName, "r");
  uint32_t n;
  uint64_t idx = 0;
  PARSER   *PA = CreateParser();
  CBUF     *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  uint8_t  sym, irSym, *readBuf;
  FileType(PA, Reader);
  fclose(Reader);
  struct   stat s;
  size_t   size, k;
  long     fd;

  if((fd = open(refName, O_RDONLY)) == -1){
    fprintf(stderr, "[x] Error: unable to open file\n");
    exit(1);
    }

  fstat (fd, &s);
  size = s.st_size;
  readBuf = (uint8_t *) mmap(0, size, PROT_READ, MAP_PRIVATE|MAP_POPULATE, 
  fd, 0);
  madvise(readBuf, s.st_size, MADV_SEQUENTIAL);
  for(k = 0 ; k < size ; ++k){
    if(ParseSym(PA, (sym = *readBuf++)) == -1){ idx = 0; continue; }
    symBuf->buf[symBuf->idx] = sym = DNASymToNum(sym);
    for(n = 0 ; n < P->nModels ; ++n){
      CModel *CM = Models[n];
      GetPModelIdx(symBuf->buf+symBuf->idx-1, CM);
      if(++idx > CM->ctx){
        UpdateCModelCounter(CM, sym, CM->pModelIdx);
        if(CM->ir == 1){                         // INVERTED REPEATS
          irSym = GetPModelIdxIR(symBuf->buf+symBuf->idx, CM);
          UpdateCModelCounter(CM, irSym, CM->pModelIdxIR);
          }
        }
      }
    UpdateCBuffer(symBuf);
    }
 
  for(n = 0 ; n < P->nModels ; ++n)
    ResetCModelIdx(Models[n]);
  RemoveCBuffer(symBuf);
  RemoveParser(PA);
  close(fd);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - C O M P R E S S O R - - - - - - - - - - - - - -

void CompressAction(Threads *T, char *refName, char *baseName){
  pthread_t t[P->nThreads];
  uint32_t n;

  Models = (CModel **) Malloc(P->nModels * sizeof(CModel *));
  for(n = 0 ; n < P->nModels ; ++n)
    Models[n] = CreateCModel(T[0].model[n].ctx, T[0].model[n].den, 
    T[0].model[n].ir, REFERENCE, P->col, T[0].model[n].edits, 
    T[0].model[n].eDen);

  fprintf(stderr, "  [+] Loading metagenomic file ... ");
  LoadReference(refName);
  fprintf(stderr, "Done!\n");

  fprintf(stderr, "  [+] Compressing database ... ");
  for(n = 0 ; n < P->nThreads ; ++n)
    pthread_create(&(t[n+1]), NULL, CompressThread, (void *) &(T[n]));
  for(n = 0 ; n < P->nThreads ; ++n) // DO NOT JOIN FORS!
    pthread_join(t[n+1], NULL);
  fprintf(stderr, "Done!\n");

  for(n = 0 ; n < P->nModels ; ++n)
    FreeCModel(Models[n]);
  Free(Models);
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

  P->col       = ArgsNum    (col,   p, argc, "-c", 1, 200);
  P->gamma     = ArgsDouble (gamma, p, argc, "-g");
  P->gamma     = ((int)(P->gamma * 65536)) / 65536.0;
  P->output    = ArgsFileGen(p, argc, "-x", "top", ".csv");

  FILE *OUTPUT = NULL;
  if(!P->force) 
    FAccessWPerm(P->output);
  OUTPUT = Fopen(P->output, "w");

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
  StopTimeNDRM(Time, clock());
  fprintf(stderr, "\n");

  fprintf(stderr, "==[ RESULTS ]=======================\n");

  k = 0;
  P->top = CreateTop(topSize * P->nThreads);
  for(ref = 0 ; ref < P->nThreads ; ++ref){
    for(n = 0 ; n < T[ref].top->size-1 ; ++n){
      P->top->V[k].value = T[ref].top->V[n].value; 
      P->top->V[k].size  = T[ref].top->V[n].size; 
      CopyStringPart(P->top->V[k].name, T[ref].top->V[n].name);
      ++k;
      }
    }
  
  fprintf(stderr, "  [+] Sorting ... ");
  qsort(P->top->V, k, sizeof(VT), SortByValue);
  fprintf(stderr, "Done!\n");

  fprintf(stderr, "  [+] Printing to file: %s ... ", P->output);
  PrintTop(OUTPUT, P->top, topSize);
  fprintf(stderr, "Done!\n");

  if(P->verbose && topSize <= 100){
    PrintTopInfo(P->top, topSize);
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
  if(!OUTPUT) fclose(OUTPUT);

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
