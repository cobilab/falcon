#include "top.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int SortByValue(const void *a, const void *b){ 
  VT *ia = (VT *) a;
  VT *ib = (VT *) b;
  if(ia->value < ib->value)      return -1;
  else if(ia->value > ib->value) return 1;
  else                           return 0;
  } 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

TOP *CreateTop(uint32_t size){
  uint32_t n;
  TOP *T  = (TOP *) Calloc(1, sizeof(TOP));
  T->size = size;
  T->V    = (VT  *) Calloc(size, sizeof(VT));
  for(n = 0 ; n < size ; ++n)
    T->V[n].name = (uint8_t *) Calloc(MAX_NAME, sizeof(uint8_t));
  return T;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateTop(double bits, uint8_t *nm, TOP *T){
  uint32_t r;
  if(T->id < T->size){
    T->V[T->id].value = bits;
    for(r = 0 ; r < MAX_NAME-1 ; ++r){
      T->V[T->id].name[r] = nm[r];
      if(nm[r] == '\0') break;
      }
    }
  else{
/*
    if(bits > T->V[0].value || T->id == T->size){
      if(T->id == T->size){
        // ORDER ALL
*/
        qsort(T->V, T->size, sizeof(VT), SortByValue);
/*
        }
      else{
        // ORDER ONLY BY ONE ELEMENT
        }
      }
*/
    }
  T->id++;
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
