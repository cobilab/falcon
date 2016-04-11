#ifndef PMODELS_H_INCLUDED
#define PMODELS_H_INCLUDED

#include "defs.h"
#include "buffer.h"
#include "models.h"

typedef struct{
  U32        *freqs;
  U32        sum;
  }
PModel;

typedef struct{
  double     *freqs;
  }
FloatPModel;

typedef struct{
  uint32_t   totModels;
  double     *weight;
  double     totalWeight;
  }
CMWeight;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

PModel          *CreatePModel        (U32);
FloatPModel     *CreateFloatPModel   (U32);
void            RemovePModel         (PModel *);
void            RemoveFPModel        (FloatPModel *);
inline void     ComputeMXProbs       (FloatPModel *, PModel *);
inline void     ComputeWeightedFreqs (double, PModel *, FloatPModel *);
double          PModelSymbolLog      (PModel *, U32);
CMWeight        *CreateWeightModel   (uint32_t);
void            ResetWeightModel     (CMWeight *);
void            RenormalizeWeights   (CMWeight *);
void            CalcDecayment        (CMWeight *, PModel **, uint8_t, double);
void            DeleteWeightModel    (CMWeight *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
