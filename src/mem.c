#include "mem.h"
#include "defs.h"
#include <math.h>
#include <stdlib.h>

static uint64_t totalMemory = 0;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void *Malloc(size_t size){
  void *pointer = malloc(size);
  if(pointer == NULL){
    fprintf(stderr, "Error allocating %zu bytes.\n", size);
    exit(1);
    }
  totalMemory += size;
  return pointer;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void *Calloc(size_t nmemb, size_t size){
  void *pointer = calloc(nmemb, size);
  if(pointer == NULL){
    fprintf(stderr, "Error allocating %zu bytes.\n", size);
    exit(1);
    }
  totalMemory += nmemb * size;
  return pointer;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void *Realloc(void *ptr, size_t size, size_t addSize){
  void *pointer = realloc(ptr, size);	
  if(pointer == NULL){
    fprintf(stderr, "Error allocating %zu bytes.\n", size);
    exit(1);
    }
  totalMemory += addSize;
  return pointer;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Free(void *ptr){
  free(ptr);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t TotalMemory(){
  return totalMemory;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
