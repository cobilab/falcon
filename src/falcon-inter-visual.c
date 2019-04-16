#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "mem.h"
#include "defs.h"
#include "msg.h"
#include "buffer.h"
#include "common.h"
#include "paint.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - R E A D   L A B E L S - - - - - - - - - - - - - 

void ReadLabels(void){
  if(access(P->labels, F_OK ) == -1)
    return;
  FILE *F = Fopen(P->labels, "r");
  uint32_t n;
  for(n = 0 ; n < P->nFiles ; ++n){
    if(!fscanf(F, "%s", P->files[n])){
      fclose(F);
      return;
      }
    }
  fclose(F);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - M A T R I X   S I Z E - - - - - - - - - - - - - 

void ReadMatrixSize(char *fn){
  FILE *F = Fopen(fn, "r");
  char c;
  P->nFiles = 0;
  while((c = fgetc(F)) != '\n'){
    if(c == EOF){
      fclose(F);
      break;
      }
    if(c == '\t')
      ++P->nFiles;
    }
  if(P->nFiles == 0){
    fprintf(stderr, "[x] Error: invalid file!\n");
    exit(1);
    }
  fprintf(stderr, "  [>] Matrix size: %u\n", P->nFiles);
  fclose(F);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - M A T R I X   S I Z E - - - - - - - - - - - - -

void ReadMatrix(char *fn){
  FILE *F = Fopen(fn, "r");
  uint32_t n, k;
  for(n = 0 ; n < P->nFiles ; ++n){
    for(k = 0 ; k < P->nFiles ; ++k){
      if(!fscanf(F, "%lf", &P->matrix[n][k])){
        fclose(F); 
        return;
        }
      }
    }
  fclose(F);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - P A I N T - - - - - - - - - - - - - - - -

void PaintMatrix(double start, double rotations, double hue, double gamma,
double width, double space){
  FILE *Plot = Fopen(P->image, "w");
  Painter *Paint;
  COLORS  *CLR = (COLORS *) Calloc(1, sizeof(COLORS));
  uint32_t ref, tar;

  CLR->start     = start;
  CLR->rotations = rotations;
  CLR->hue       = hue;
  CLR->gamma     = gamma;

  Paint = CreateBasicPainter(DEFAULT_CX*2+((width+space)*P->nFiles)+5, width, space);

  PrintHead(Plot, (2 * DEFAULT_CX) + (((Paint->width + Paint->space) * 
  P->nFiles) - Paint->space), Paint->size + EXTRA);

  Rect(Plot, (2 * DEFAULT_CX) + (((Paint->width + Paint->space) * P->nFiles) - 
  Paint->space), Paint->size + EXTRA, 0, 0, "#ffffff");

  // PRINT HEATMAP SCALE
  uint32_t size = (Paint->width + Paint->space) * P->nFiles - Paint->space;
  for(ref = 0 ; ref < size ; ++ref){
    char color[12];
    Rect(Plot, Paint->width, 1, DEFAULT_CX - (Paint->width*2), Paint->cy + ref, 
    HeatMapColor(((double) ref / size), color, CLR));
    }
  if(P->nFiles > 4)
    Text90d(Plot, -DEFAULT_CX - ((size/2)+46), DEFAULT_CX-(Paint->width*2 + 2), 
    "SIMILARITY");
  Text   (Plot, DEFAULT_CX-(Paint->width*2 + 14), Paint->cy+13, "+");
  Text   (Plot, DEFAULT_CX-(Paint->width*2 + 14), Paint->cy+size, "-");

  for(ref = 0 ; ref < P->nFiles ; ++ref){
    //for(tar = P->nFiles ; tar-- ; ){ // INVERT LOOP TO INVERT HORIZONTAL ORDER
    for(tar = 0 ; tar < P->nFiles ; ++tar){ 
      char color[12];
      Rect(Plot, Paint->width, Paint->width, Paint->cx, Paint->cy, 
      HeatMapColor(BoundDouble(0.0, P->matrix[ref][tar], 1.0), color, CLR));
      Paint->cx += Paint->width + Paint->space;
      }
    // TEXT HAS 16 PX -> CALCULATE AVERAGE POSITION
    Text   (Plot, Paint->cx + 4, (Paint->cy+Paint->width/2)+6, P->files[ref]);

    // USE THE FOLLOWING TO INVERT THE NAMES ORDER -> THEN CHANGE IN THE SQUARES
    //Text90d(Plot, 4-DEFAULT_CX, (Paint->cy+Paint->width/2)+10, P->files[P->nFiles-1-ref]);
    Text90d(Plot, 4-DEFAULT_CX, (Paint->cy+Paint->width/2)+10, P->files[ref]);

    Paint->cx =  DEFAULT_CX;
    Paint->cy += Paint->width + Paint->space;
    }
  
  PrintFinal(Plot);
  RemovePainter(Paint);
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - M A I N - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t main(int argc, char *argv[]){
  char        **p = *&argv;
  uint32_t    n; 
  double      start, rotations, hue, gamma, width, space;
  
  P = (Parameters *) Malloc(1 * sizeof(Parameters));
  if((P->help = ArgsState(DEFAULT_HELP, p, argc, "-h")) == 1 || argc < 2){
    PrintMenuInterVisual();
    return EXIT_SUCCESS;
    }

  if(ArgsState(DEF_VERSION, p, argc, "-V")){
    PrintVersion();
    return EXIT_SUCCESS;
    }

  P->verbose  = ArgsState    (DEFAULT_VERBOSE, p, argc, "-v");
  P->force    = ArgsState    (DEFAULT_FORCE,   p, argc, "-F");
  P->image    = (uint8_t *)   ArgsFilesImg    (p, argc, "-x");
  P->labels   = (uint8_t *)   ArgsFile        (p, argc, "-l");
  width       = ArgsDouble   (DEFAULT_WIDTH,   p, argc, "-w");
  space       = ArgsDouble   (DEFAULT_SPACE,   p, argc, "-a");
  start       = ArgsDouble   (0.35,            p, argc, "-s");
  rotations   = ArgsDouble   (1.50,            p, argc, "-r");
  hue         = ArgsDouble   (1.92,            p, argc, "-u");
  gamma       = ArgsDouble   (0.50,            p, argc, "-g");

  fprintf(stderr, "\n");
  if(P->verbose){
    fprintf(stderr, "==[ CONFIGURATION ]=================\n");
    fprintf(stderr, "Verbose mode ....................... yes\n");
    fprintf(stderr, "Heatmap design characteristics:\n");
    fprintf(stderr, "  [+] Width ........................ %.3g\n", width);
    fprintf(stderr, "  [+] Space ........................ %.3g\n", space);
    fprintf(stderr, "  [+] Start ........................ %.3g\n", start);
    fprintf(stderr, "  [+] Rotations .................... %.3g\n", rotations);
    fprintf(stderr, "  [+] Hue .......................... %.3g\n", hue);
    fprintf(stderr, "  [+] Gamma ........................ %.3g\n", gamma);
    fprintf(stderr, "Input labels filename .............. %s\n",   P->labels);
    fprintf(stderr, "Output heatmap filename ............ %s\n",   P->image);
    fprintf(stderr, "\n");
    }

  fprintf(stderr, "==[ PROCESSING ]====================\n");
  ReadMatrixSize(argv[argc-1]);
  P->matrix = (double  **) Calloc(P->nFiles, sizeof(double *));
  P->files  = (char    **) Calloc(P->nFiles, sizeof(char   *));
  for(n = 0 ; n < P->nFiles ; ++n){
    P->matrix[n] = (double *) Calloc(P->nFiles, sizeof(double));
    P->files [n] = (char   *) Calloc(MAX_LABEL, sizeof(char  ));
    P->files[n][0]  = '?'; 
    P->files[n][1]  = '\0'; 
    }
  fprintf(stderr, "  [+] Loading labels ... ");
  ReadLabels();
  fprintf(stderr, "Done!\n");
  fprintf(stderr, "  [+] Loading matrix ... ");
  ReadMatrix(argv[argc-1]);
  fprintf(stderr, "Done!\n");
  fprintf(stderr, "  [+] Painting heatmap ... ");
  PaintMatrix(start, rotations, hue, gamma, width, space);
  fprintf(stderr, "Done!\n\n");

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
