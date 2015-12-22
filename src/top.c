#include "top.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CopyStringPart(uint8_t *a, uint8_t *b){
  uint32_t r;
  for(r = 0 ; r < MAX_NAME-1 ; ++r){
    a[r] = b[r];
    if(b[r] == '\0') 
      break;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int SortByValue(const void *a, const void *b){ 
  VT *ia = (VT *) a;
  VT *ib = (VT *) b;
  if     (ia->value < ib->value) return -1;
  else if(ia->value > ib->value) return 1;
  else                           return 0;
  } 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

TOP *CreateTop(uint32_t size){
  uint32_t n;
  TOP *T  = (TOP *) Calloc(1, sizeof(TOP));
  T->size = size + 1;
  T->V    = (VT  *) Calloc(T->size, sizeof(VT));
  for(n = 0 ; n < T->size ; ++n){
    T->V[n].value = 1.0;
    T->V[n].name  = (uint8_t *) Calloc(MAX_NAME, sizeof(uint8_t));
    T->V[n].size  = 1;
    }
  return T;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void AddElement(VT *Vt, double value, uint8_t *nm, uint64_t size){
  Vt->value = value;
  CopyStringPart(Vt->name, nm);
  Vt->size  = size;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateTop(double bits, uint8_t *nm, TOP *T, uint64_t size){
  uint32_t last = T->size - 1;
  if(T->id < last){
    AddElement(&T->V[T->id], bits, nm, size);
    qsort(T->V, T->id+1, sizeof(VT), SortByValue); 
    }
  else if(T->id == last){
    AddElement(&T->V[last], bits, nm, size);
    qsort(T->V, T->size, sizeof(VT), SortByValue);
    }
  else{ // real NRC = 1.0-bits
    if(T->V[last].value > bits){
      AddElement(&T->V[last], bits, nm, size);
      qsort(T->V, T->size, sizeof(VT), SortByValue);
      }
    }
  T->id++;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintTop(FILE *F, TOP *Top, uint32_t size){
  uint32_t n;
  if(size > Top->size){
    fprintf(stderr, "  [x] Error: top is larger than size!\n");
    exit(1);
    }
  for(n = 0 ; n < size ; ++n)
    fprintf(F, "%u\t%"PRIu64"\t%12.9g\t%s\n", n+1, Top->V[n].size, 
    (1.0-Top->V[n].value)*100.0, Top->V[n].name);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintTopInfo(TOP *Top, uint32_t size){
  uint32_t n;
  if(size > Top->size){
    fprintf(stderr, "  [x] Error: top is larger than size!\n");
    exit(1);
    }
  fprintf(stderr, "  [*] Top %u:\n", size);
  for(n = 0 ; n < size ; ++n)
    fprintf(stderr, "  [*] %u \t%"PRIu64"\t%12.9g\t%s\n", n+1, Top->V[n].size,
    (1.0-Top->V[n].value)*100.0, Top->V[n].name);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void DeleteTop(TOP *T){
  uint32_t n;
  for(n = 0 ; n < T->size ; ++n)
    Free(T->V[n].name);
  Free(T->V);
  Free(T);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
