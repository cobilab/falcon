#ifndef PAINT_H_INCLUDED
#define PAINT_H_INCLUDED

#include "defs.h"

#define DEFAULT_CX             70.0
#define DEFAULT_CY             75.0
#define DEFAULT_TX             50.0                        // X COORD FOR TEXT
#define DEFAULT_TY             82.0                        // Y COORD FOR TEXT
#define DEFAULT_WIDTH          20.0                   // CHROMOSOMES THICKNESS
#define DEFAULT_SPACE          10                 // SPACE BETWEEN CHROMOSOMES
#define DEFAULT_SCALE          100000                 // SCALE FOR CHROMOSOMES
#define EXTRA                  150                   // EXTRA SPACE FOR BORDER
#define LEVEL_SATURATION       220 
#define LEVEL_VALUE            160
#define LEVEL_HUE              150
#define LEVEL_HUE_N            1                              // N's HUE COLOR

typedef struct{
  char    *backColor;
  double  width;
  double  space;
  double  cx; 
  double  cy;
  double  tx;
  double  ty;
  double  size;  
  }
Painter;

typedef struct{
  double start;
  double rotations;
  double hue;
  double gamma;
  }
COLORS;

typedef struct{
  uint8_t  r;
  uint8_t  g;
  uint8_t  b;
  } 
RgbColor;

typedef struct{
  uint8_t  h;
  uint8_t  s;
  uint8_t  v;
  } 
HsvColor;

double scale;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char      *HeatMapColor  (double, char *, COLORS *);
Painter   *CreatePainter (double, double, char *);
void      RemovePainter  (Painter *);
RgbColor  HsvToRgb       (HsvColor);
HsvColor  RgbToHsv       (RgbColor);
char      *GetRgbColor   (uint8_t);
void      PrintFinal     (FILE *);
void      PrintHead      (FILE *, double, double);
void      RectOval       (FILE *, double, double, double, double, char *);
void      RectOvalIR     (FILE *, double, double, double, double, char *);
void      Rect           (FILE *, double, double, double, double, char *);
void      RectIR         (FILE *, double, double, double, double, char *);
void      Chromosome     (FILE *, double, double, double, double);
void      Text           (FILE *, double, double, char *);
void      TextFloat      (FILE *, double, double, double);
void      SetScale       (double);
double    GetPoint       (uint64_t);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
