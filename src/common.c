#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "defs.h"
#include "mem.h"
#include "common.h"
#include "msg.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Fclose(FILE *F){
  if(F == NULL){
    fprintf(stderr, "Error (Fclose): NULL file pointer.\n");
    exit(EXIT_FAILURE);
    }

  if(ferror(F)){
    fprintf(stderr, "Error (Fclose): unable to close the stream due to an "
    "unknown error.\n");
    exit(EXIT_FAILURE);
    }
		
  if(fclose(F) != 0){
    fprintf(stderr, "Error (Fclose): unable to close the file stream.\n");
    exit(EXIT_FAILURE);	
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Fseeko(FILE *F, off_t offset, int32_t ac){

  if(F == NULL){
    fprintf(stderr, "Error (Fseeko): NULL file pointer.\n");
    exit(EXIT_FAILURE);
    }
		
  if(fseeko(F, offset, ac) != 0){
    Fclose(F);
    fprintf(stderr, "Error (Fseeko): unable to change file position.\n");
    fprintf(stderr, "Error (Fseeko): offset = %"PRId64".\n",offset);
		
    switch(ac){
      case SEEK_SET:
      fprintf(stderr, "Error (Fseeko): origin = %d (SEEK_SET).\n", ac);
      break;
      case SEEK_CUR:
      fprintf(stderr, "Error (Fseeko): origin = %d (SEEK_CUR).\n", ac);
      break;
      case SEEK_END:
      fprintf(stderr, "Error (Fseeko): origin = %d (SEEK_END).\n", ac);
      default:
      fprintf(stderr, "Error (Fseeko): invalid origin value = %d.\n", ac);
      exit(EXIT_FAILURE);
      }			
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	
	
uint64_t Ftello(FILE *F){
  if(F == NULL){
    fprintf(stderr, "Error (Ftello): NULL file pointer.\n");
    exit(EXIT_FAILURE);
    }
	
  if(ftello(F) < 0){
    Fclose(F);
    fprintf(stderr, "Error (Ftello): unable get the current position of the"
    " file associated with stream.\n");
    exit(EXIT_FAILURE);	
    }
	
  return (uint64_t) ftello(F);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double BoundDouble(double low, double value, double high){
  if(value < low ) return low;
  if(value > high) return high;
  return value;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double BPBB(double bits, uint64_t nBase){
  return BoundDouble(0.0, bits / 2.0 / nBase, 1.0);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

unsigned QuadQuantization(double v){
  if     (v >= 2.00) return 8;
  else if(v >= 1.75) return 7;
  else if(v >= 1.50) return 6;
  else if(v >= 1.25) return 5;
  else if(v >= 1.00) return 4;
  else if(v >= 0.75) return 3;
  else if(v >= 0.50) return 2;
  else if(v >= 0.25) return 1;
  else               return 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t PackByte(double v, uint8_t sym){
  switch(sym){
    case 0: 
      switch(QuadQuantization(v)){
        case 0: return 0x20;
        case 1: return 0x21;
        case 2: return 0x22;
        case 3: return 0x23;
        case 4: return 0x24;
        case 5: return 0x25;
        case 6: return 0x26;
        case 7: return 0x27;
        case 8: return 0x28;
        case 9: return 0x29;
        default: break;
        }
    break;
    case 1:
      switch(QuadQuantization(v)){
        case 0: return 0x2A;
        case 1: return 0x2B;
        case 2: return 0x2C;
        case 3: return 0x2D;
        case 4: return 0x2E;
        case 5: return 0x2F;
        case 6: return 0x30;
        case 7: return 0x31;
        case 8: return 0x32;
        case 9: return 0x33;
        default: break;
        }
    break;
    case 2:
      switch(QuadQuantization(v)){
        case 0: return 0x34;
        case 1: return 0x35;
        case 2: return 0x36;
        case 3: return 0x37;
        case 4: return 0x38;
        case 5: return 0x39;
        case 6: return 0x3A;
        case 7: return 0x3B;
        case 8: return 0x3C;
        case 9: return 0x3D;
        default: break;
        }
    break;
    case 3:
      switch(QuadQuantization(v)){
        case 0: return 0x3E;
        case 1: return 0x3F;
        case 2: return 0x40;
        case 3: return 0x41;
        case 4: return 0x42;
        case 5: return 0x43;
        case 6: return 0x44;
        case 7: return 0x45;
        case 8: return 0x46;
        case 9: return 0x47;
        default: break;
        }
    break;
    case 4:
      switch(QuadQuantization(v)){
        case 0: return 0x48;
        case 1: return 0x49;
        case 2: return 0x4A;
        case 3: return 0x4B;
        case 4: return 0x4C;
        case 5: return 0x4D;
        case 6: return 0x4E;
        case 7: return 0x4F;
        case 8: return 0x50;
        case 9: return 0x51;
        default: break;
        }
    break;
    default: break;
    }
  fprintf(stderr, "  [x] Error: PackByte!\n"); 
  exit(1);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

SymValue UnPackByte(uint8_t b){
  SymValue SM;
  switch(b){
    case 0x21: SM.sym = 0; SM.value = 0; break;
    case 0x22: SM.sym = 0; SM.value = 1; break;
    case 0x23: SM.sym = 0; SM.value = 2; break;
    case 0x24: SM.sym = 0; SM.value = 3; break;
    case 0x25: SM.sym = 0; SM.value = 4; break;
    case 0x26: SM.sym = 0; SM.value = 5; break;
    case 0x27: SM.sym = 0; SM.value = 6; break;
    case 0x28: SM.sym = 0; SM.value = 7; break;
    case 0x29: SM.sym = 0; SM.value = 8; break;
    case 0x2A: SM.sym = 0; SM.value = 9; break;
    case 0x2B: SM.sym = 1; SM.value = 0; break;
    case 0x2C: SM.sym = 1; SM.value = 1; break;
    case 0x2D: SM.sym = 1; SM.value = 2; break;
    case 0x2E: SM.sym = 1; SM.value = 3; break;
    case 0x2F: SM.sym = 1; SM.value = 4; break;
    case 0x30: SM.sym = 1; SM.value = 5; break;
    case 0x31: SM.sym = 1; SM.value = 6; break;
    case 0x32: SM.sym = 1; SM.value = 7; break;
    case 0x33: SM.sym = 1; SM.value = 8; break;
    case 0x34: SM.sym = 1; SM.value = 9; break;
    case 0x35: SM.sym = 2; SM.value = 0; break;
    case 0x36: SM.sym = 2; SM.value = 1; break;
    case 0x37: SM.sym = 2; SM.value = 2; break;
    case 0x38: SM.sym = 2; SM.value = 3; break;
    case 0x39: SM.sym = 2; SM.value = 4; break;
    case 0x3A: SM.sym = 2; SM.value = 5; break;
    case 0x3B: SM.sym = 2; SM.value = 6; break;
    case 0x3C: SM.sym = 2; SM.value = 7; break;
    case 0x3D: SM.sym = 2; SM.value = 8; break;
    case 0x3E: SM.sym = 2; SM.value = 9; break;
    case 0x3F: SM.sym = 3; SM.value = 0; break;
    case 0x40: SM.sym = 3; SM.value = 1; break;
    case 0x41: SM.sym = 3; SM.value = 2; break;
    case 0x42: SM.sym = 3; SM.value = 3; break;
    case 0x43: SM.sym = 3; SM.value = 4; break;
    case 0x44: SM.sym = 3; SM.value = 5; break;
    case 0x45: SM.sym = 3; SM.value = 6; break;
    case 0x46: SM.sym = 3; SM.value = 7; break;
    case 0x47: SM.sym = 3; SM.value = 8; break;
    case 0x48: SM.sym = 3; SM.value = 9; break;
    case 0x49: SM.sym = 4; SM.value = 0; break;
    case 0x4A: SM.sym = 4; SM.value = 1; break;
    case 0x4B: SM.sym = 4; SM.value = 2; break;
    case 0x4C: SM.sym = 4; SM.value = 3; break;
    case 0x4D: SM.sym = 4; SM.value = 4; break;
    case 0x4E: SM.sym = 4; SM.value = 5; break;
    case 0x4F: SM.sym = 4; SM.value = 6; break;
    case 0x50: SM.sym = 4; SM.value = 7; break;
    case 0x51: SM.sym = 4; SM.value = 8; break;
    case 0x52: SM.sym = 4; SM.value = 9; break;
    default: fprintf(stderr, "  [x] Error: unpacking byte!\n");
    exit(1);
    }
  
  return SM;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t SumWriteBits(uint8_t **bin, int tar, uint64_t min,  FILE *Writter, 
FILE *DNA){
  int x, ref, sum;

  for(x = 0 ; x < 8 ; ++x){
    char c = fgetc(DNA); //FIXME: extra chars
    sum = 0;
    for(ref = 0 ; ref < P->nFiles ; ++ref)
      if(ref != tar)
        sum += (int) bin[ref][x];

    if(sum < P->index)
      min = 0;
    else if(++min >= P->blockSize)
      fprintf(Writter, "%c", c);
    }

  return min;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

float *logTable;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint32_t FLog2(uint64_t i)
  {
  uint32_t n, m, k = 32, o = (i & (i - 1)) ? 1 : 0;
  static const uint64_t sizes[6] = 
  { 0x0000000000000002ull, 0x000000000000000Cull, 0x00000000000000F0ull, 
    0x000000000000FF00ull, 0x00000000FFFF0000ull, 0xFFFFFFFF00000000ull };

  for(n = 6 ; n-- ; )
    {
    o += (m = (i & *(sizes+n)) ? k : 0);
    i >>= m;
    k >>= 1;
    }

  return o;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//        Pow function from http://martin.ankerl.com/2007/10/04/
//        optimized-pow-approximation-for-java-and-c-c/

double Power(double a, double b)
  {
  int tmp = (*(1 + (int *)&a));
  int tmp2 = (int)(b * (tmp - 1072632447) + 1072632447);
  double p = 0.0;
  *(1 + (int * )&p) = tmp2;
  return p;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ShiftBuffer(uint8_t *buf, int size, uint8_t new){
  memmove(buf, buf + 1, size * sizeof(uint8_t));
  buf[size - 1] = new;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// REPLACE STRING
//
char *ReplaceSubStr(char *str, char *a, char *b){
  char *buf = (char *) Calloc(MAX_STR, sizeof(char));
  char *p;
  if(strlen(str) > MAX_STR){
    fprintf(stderr, "[x] Error: string too long!\n");
    exit(1);
    }
  if(!(p = strstr(str, a)))
    return str;
  strncpy(buf, str, p-str);
  buf[p-str] = '\0';
  sprintf(buf+(p-str), "%s%s", b, p+strlen(a));
  return buf;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t NBytesInFile(FILE *file){
  uint64_t size = 0;
  fseeko(file, 0, SEEK_END);
  size = ftello(file);
  rewind(file);
  return size;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t NDNASyminFile(FILE *file){
  uint8_t  buffer[BUFFER_SIZE];
  uint32_t k, idx;
  uint64_t nSymbols = 0;

  while((k = fread(buffer, 1, BUFFER_SIZE, file)))
    for(idx = 0 ; idx < k ; ++idx)
      switch(buffer[idx]){
        case 'A': ++nSymbols; break;
        case 'T': ++nSymbols; break;
        case 'C': ++nSymbols; break;
        case 'G': ++nSymbols; break;
        }

  rewind(file);
  return nSymbols;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t NDNASymInFasta(FILE *file){
  uint8_t  buffer[BUFFER_SIZE], sym = 0, header = 1;
  uint32_t k, idx;
  uint64_t nSymbols = 0;

  while((k = fread(buffer, 1, BUFFER_SIZE, file)))
    for(idx = 0 ; idx < k ; ++idx){
      sym = buffer[idx];
      if(sym == '>'){ header = 1; continue; }
      if(sym == '\n' && header == 1){ header = 0; continue; }
      if(sym == '\n') continue;
      if(sym == 'N' ) continue;
      if(header == 1) continue;
      // FINAL FILTERING DNA CONTENT
      if(sym != 'A' && sym != 'C' && sym != 'G' && sym != 'T')
        continue;
      ++nSymbols;
      }

  rewind(file);
  return nSymbols;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t NDNASymInFastq(FILE *file){
  uint8_t  buffer[BUFFER_SIZE], sym = 0, line = 0, dna = 0;
  uint32_t k, idx;
  uint64_t nSymbols = 0;

  while((k = fread(buffer, 1, BUFFER_SIZE, file)))
    for(idx = 0 ; idx < k ; ++idx){
      sym = buffer[idx];

      switch(line){
        case 0: if(sym == '\n'){ line = 1; dna = 1; } break;
        case 1: if(sym == '\n'){ line = 2; dna = 0; } break;
        case 2: if(sym == '\n'){ line = 3; dna = 0; } break;
        case 3: if(sym == '\n'){ line = 0; dna = 0; } break;
        }
      if(dna == 0 || sym == '\n') continue;
      if(dna == 1 && sym == 'N' ) continue;

      // FINAL FILTERING DNA CONTENT
      if(sym != 'A' && sym != 'C' && sym != 'G' && sym != 'T')
        continue;
      ++nSymbols;
      }

  rewind(file);
  return nSymbols;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int FileExists(const char *fn){
  FILE *F = NULL;
  if((F = fopen(fn, "r")) != NULL){ 
    fclose(F);
    return 1;
    }
  return 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t FopenBytesInFile(const char *fn){
  uint64_t size = 0;
  FILE *file = Fopen(fn, "r");
  size = NBytesInFile(file);  
  fclose(file);
  return size;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void FillLogTable(uint32_t nSym, uint32_t alphaDen, uint32_t maxCHigh)
  {
  uint32_t n, maxSize = nSym * maxCHigh * alphaDen;
 
  logTable = (float *) Malloc(maxSize * sizeof(float));
  for(n = 1 ; n != maxSize ; ++n)
    logTable[n] = FLog2(n);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double SearchLog(uint32_t idx)
  {
  return logTable[idx];
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t DNASymToNum(uint8_t symbol)
  {
  switch(symbol)
    {
    case 'A': return 0;
    case 'a': return 0;
    case 'T': return 3;
    case 't': return 3;
    case 'U': return 3;
    case 'C': return 1;
    case 'c': return 1;
    case 'G': return 2;
    case 'g': return 2;
    default : return 4;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t NumToDNASym(uint8_t symbol)
  {
  switch(symbol)
    {
    case 0: return 'A';
    case 3: return 'T';
    case 1: return 'C';
    case 2: return 'G';
    default: fprintf(stderr, "Error: unknown numerical symbols\n"); exit(1);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t GetCompSym(uint8_t symbol)
  {
  switch(symbol)
    {
    case 'A': return 'T';
    case 'C': return 'G';
    case 'G': return 'C';
    case 'T': return 'A';
    default:  return symbol;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t GetCompNum(uint8_t symbol)
  {
  switch(symbol)
    {
    case 0: return 3;
    case 1: return 2;
    case 2: return 1;
    case 3: return 0;
    default:  fprintf(stderr, "symbol\n");
    return symbol;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FILE *Fopen(const char *path, const char *mode)
  {
  FILE *file = fopen(path, mode);

  if(file == NULL)
    {
    fprintf(stderr, "Error opening: %s (mode %s). Does the file exist?\n", 
    path, mode);
    exit(1);
    }

  return file;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

uint8_t *ReverseStr(uint8_t *str, uint32_t end)
  {
  uint32_t start = 0;

  while(start < end)
    {
    str[start] ^= str[end];
    str[end]   ^= str[start];
    str[start] ^= str[end];
    ++start;
    --end;
    }

  return str;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SortString(char str[])
  {
  char     tmp;
  int32_t  i, j, length = strlen(str);

  for(i = 0 ; i != length-1 ; ++i)
    for (j = i+1 ; j != length ; ++j)
      if(str[i] > str[j])
        {
        tmp    = str[i];
        str[i] = str[j];
        str[j] = tmp;
        }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *CloneString(char *str)
  {
  char *clone;
  if(str == NULL)
    return NULL;
  strcpy((clone = (char*) Malloc((strlen(str) + 1) * sizeof(char))), str);
  return clone;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *concatenate(char *a, char *b)
  {
  char *base;

  base = (char *) Malloc(strlen(a) + strlen(b) + 1);
  strcpy(base, a);
  strcat(base, b);
  return base;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *RepString(const char *str, const char *old, const char *new)
  {
  size_t sLen = strlen(str) + 1;
  char *cout = 0, *p = 0, *tmp; 

  if(!(p = (cout = (char *) Malloc(sLen * sizeof(char)))))
    return 0;
  while(*str)
    if((*str & 0xc0) != 0x80 && !strncmp(str, old, strlen(old)))
      {
      p   -= (intptr_t) cout;
      tmp  = strcpy(p = (cout = (char *) Realloc(cout, sLen += strlen(new) - 
             strlen(old), strlen(new) - strlen(old))) + (intptr_t) p, new);
      p   += strlen(tmp);
      str += strlen(old);
      }
    else
      *p++ = *str++;
  *p = 0;
  return cout;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint32_t ArgsNum(uint32_t d, char *a[], uint32_t n, char *s, uint32_t l,
uint32_t u){
  uint32_t x;
  for( ; --n ; ) if(!strcmp(s, a[n])){
    if((x = atol(a[n+1])) < l || x > u){
      fprintf(stderr, "[x] Invalid number! Interval: [%u;%u].\n", l, u);
      exit(EXIT_FAILURE);
      }
    return x;
    }
  return d;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t ArgsNum64(uint64_t d, char *a[], uint32_t n, char *s, uint64_t l,
uint64_t u){
  uint64_t x;
  for( ; --n ; ) if(!strcmp(s, a[n])){
    if((x = atoll(a[n+1])) < l || x > u){
      fprintf(stderr, "[x] Invalid number! Interval: [%"PRIu64";%"PRIu64"].\n", 
      l, u);
      exit(EXIT_FAILURE);
      }
    return x;
    }
  return d;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double ArgsDouble(double def, char *arg[], uint32_t n, char *str)
  {
  for( ; --n ; )
    if(!strcmp(str, arg[n]))
      return atof(arg[n+1]);
  return def;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t ArgsState(uint8_t def, char *arg[], uint32_t n, char *str)
  {     
  for( ; --n ; )
    if(!strcmp(str, arg[n]))
      return def == 0 ? 1 : 0;
  return def;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *ArgsString(char *def, char *arg[], uint32_t n, char *str)
  {
  for( ; --n ; )
    if(!strcmp(str, arg[n]))
      return arg[n+1];
  return def;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ModelPar ArgsUniqModel(char *str, uint8_t type)
  {
  uint32_t  ctx, den, ir, edits, eDen;
  ModelPar  Mp;

  if(sscanf(str, "%u:%u:%u:%u/%u", &ctx, &den, &ir, &edits, &eDen ) == 5){
    if(ctx > MAX_CTX || ctx < MIN_CTX || den > MAX_DEN || den < MIN_DEN || 
    edits > 256 || eDen > 50000){
      fprintf(stderr, "Error: invalid model arguments range!\n");
      ModelsExplanation();
      fprintf(stderr, "\nPlease set the models according to the above " 
      "description.\n");
      exit(1);
      }
    Mp.ctx   = ctx;
    Mp.den   = den;
    Mp.ir    = ir;
    Mp.edits = edits;
    Mp.eDen  = eDen;
    return Mp;
    }
  else{
    fprintf(stderr, "Error: unknown scheme for model arguments!\n");
    ModelsExplanation();
    fprintf(stderr, "\nPlease set the models according to the above "
    "description.\n");
    exit(1);
    }

  return Mp;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *ArgsFiles(char *arg[], uint32_t argc, char *str)
  {
  int32_t n = argc;

  for( ; --n ; )
    if(!strcmp(str, arg[n]))
      return CloneString(arg[n+1]);
  
  return concatenate(concatenate(arg[argc-2], arg[argc-1]), ".svg");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void FAccessWPerm(char *fn)
  {
  if(access(fn, F_OK) != -1)
    {
    fprintf(stderr, "[x] Error: file %s already exists!\n", fn);
    if(access(fn, W_OK) != -1)
      fprintf(stderr, "[i] Note: file %s has write permission.\n[i] Tip: to "
      "force writing rerun with \"-F\" option.\n[i] Warning: on forcing, the"
      " old (%s) file will be deleted permanently.\n", fn, fn);
    exit(1);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void TestReadFile(char *fn)
  {
  FILE *f = NULL;
  f = Fopen(fn, "r");
  fclose(f);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GET STRING TO ARGV
//
int32_t StrToArgv(char *s, char ***v){
  int32_t n = 0, c = 2;
  char *d = strdup(s);   // STRDUP <=> CLONE STR
  while(d[n]) if(d[n++] == ' ') ++c;
  *v = (char **) Calloc(c, sizeof(char *));
  n = 0; (*v)[0] = d; c = 1;
  do if(d[n] == ' '){
       d[n] = '\0';
       (*v)[c++] = d+n+1;
       }
  while(d[++n]);
  return c;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void CalcProgress(uint64_t size, uint64_t i)
  {
  if(i % (size / 100) == 0 && size > 1000)
    fprintf(stderr, "Progress:%3d %%\r", (uint8_t) (i / (size / 100)));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t CmpCheckSum(uint32_t cs, uint32_t checksum)
  {
  if(checksum != cs)
    { 
    fprintf(stderr, "Error: invalid reference file!\n"
    "Compression reference checksum ................. %u\n"
    "Decompression reference checksum ............... %u\n",
    cs, checksum);
    fprintf(stderr, "Tip: rerun with correct reference file\n");
    return 1;
    }
  return 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintArgs(Parameters *P, Threads T, char *ref, char *tar, uint32_t top){
  uint32_t n;

  fprintf(stderr, "==[ CONFIGURATION ]=================\n");
  fprintf(stderr, "Verbose mode ....................... %s\n", P->verbose == 0 
  ? "no" : "yes");
  fprintf(stderr, "Force mode ......................... %s\n", P->force == 0 ? 
  "no" : "yes");
  #ifdef LOCAL_SIMILARITY
  fprintf(stderr, "Local similarity mode .............. %s\n", P->local == 0 ? 
  "no" : "yes");
  #endif
  fprintf(stderr, "Compression level .................. %u\n", P->level);
  fprintf(stderr, "Sub-sampling ....................... %u\n", P->sample);
  fprintf(stderr, "Number of threads .................. %u\n", P->nThreads);
  fprintf(stderr, "Top size ........................... %u\n", top);
  for(n = 0 ; n < P->nModels ; ++n){
    fprintf(stderr, "Reference model %d:\n", n+1);
    fprintf(stderr, "  [+] Context order ................ %u\n", 
    T.model[n].ctx);
    fprintf(stderr, "  [+] Alpha denominator ............ %u\n", 
    T.model[n].den);
    fprintf(stderr, "  [+] Inverted repeats ............. %s\n", 
    T.model[n].ir == 0 ? "no" : "yes");
    fprintf(stderr, "  [+] Allowable substitutions ...... %u\n",
    T.model[n].edits);
    if(T.model[n].edits != 0)
      fprintf(stderr, "  [+] Substitutions alpha den ...... %u\n",
      T.model[n].eDen);
    }
  fprintf(stderr, "Gamma .............................. %.2lf\n", P->gamma);
  fprintf(stderr, "Maximum Collisions ................. %u\n", P->col);
  fprintf(stderr, "Output top filename ................ %s\n", P->output);
  #ifdef LOCAL_SIMILARITY
  if(P->local == 1){
    fprintf(stderr, "Output local filename .............. %s\n", P->outLoc);
    }
  #endif
  fprintf(stderr, "Metagenomic filename ............... %s\n", ref);
  fprintf(stderr, "Database filename .................. %s\n", tar);
  fprintf(stderr, "\n");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintArgsFilter(EYEPARAM *PEYE){
  fprintf(stderr, "==[ CONFIGURATION ]=================\n");
  fprintf(stderr, "Verbose mode ....................... yes\n");
  fprintf(stderr, "Filter characteristics:\n");
  fprintf(stderr, "  [+] Window size .................. %"PRIi64"\n", 
  PEYE->windowSize);
  fprintf(stderr, "  [+] Window type .................. %d\n",        
  PEYE->windowType);
  fprintf(stderr, "  [+] Sampling ..................... %"PRIi64"\n", 
  PEYE->sampling);
  fprintf(stderr, "  [+] Threshold .................... %.3lf\n",      
  PEYE->threshold);
  fprintf(stderr, "Output filename .................... %s\n",
  PEYE->output);
  fprintf(stderr, "\n");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintArgsEye(EYEPARAM *PEYE){
  fprintf(stderr, "==[ CONFIGURATION ]=================\n");
  fprintf(stderr, "Verbose mode ....................... yes\n");
  fprintf(stderr, "Show global scale .................. %s\n", PEYE->showScale 
  == 1 ? "yes" : "no"); 
  fprintf(stderr, "Visual characteristics:\n");
  fprintf(stderr, "  [+] Width ........................ %.3g\n", PEYE->width);
  fprintf(stderr, "  [+] Space ........................ %.3g\n", PEYE->space);
  fprintf(stderr, "  [+] Start ........................ %.3g\n", PEYE->start);
  fprintf(stderr, "  [+] Rotations .................... %.3g\n",
  PEYE->rotations);
  fprintf(stderr, "  [+] Hue .......................... %.3g\n", PEYE->hue);
  fprintf(stderr, "  [+] Gamma ........................ %.3g\n", PEYE->gamma);
  fprintf(stderr, "  [+] Enlarge ...................... %"PRIu64"\n", 
  PEYE->enlarge);
  fprintf(stderr, "Output visual filename ............. %s\n",  PEYE->output);
  fprintf(stderr, "\n");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *ArgsFilesImg(char *arg[], uint32_t argc, char *str){
  int32_t n = argc;

  for( ; --n ; )
    if(!strcmp(str, arg[n]))
      return CloneString(arg[n+1]);

  return concatenate("map", ".svg");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *ArgsFile(char *arg[], uint32_t argc, char *str){
  int32_t n = argc;

  for( ; --n ; )
    if(!strcmp(str, arg[n]))
      return CloneString(arg[n+1]);

  return concatenate("labels", ".csv");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *ArgsFileGen(char *arg[], uint32_t argc, char *str, char *nm, char *ext){
  int32_t n = argc;
  for( ; --n ; )
    if(!strcmp(str, arg[n]))
      return CloneString(arg[n+1]);
  return concatenate(nm, ext);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

