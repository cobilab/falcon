#include "msg.h"
#include <stdio.h>
#include <stdlib.h>

void ModelsExplanation(void){
  fprintf(stderr,
  "                                                                         \n"
  "  -m <c>:<d>:<i>:<m/e>  reference context model (ex:-m 13:100:0:0/0),    \n"
  "  -m <c>:<d>:<i>:<m/e>  reference context model (ex:-m 18:1000:0:1/1000),\n"
  "  ...                                                                    \n"
  "                        templates use <c> for context-order size, <d> for\n"
  "                        alpha (1/<d>), <i> (0 or 1) to set the usage of  \n"
  "                        inverted repeats (set 1 to use) and <m> to the   \n"
  "                        maximum allowed mutation on the context without  \n"
  "                        being discarded (usefull in deep contexts), under\n"
  "                        the estimator <e>.                               \n"
  "                                                                       \n");
  }

void PrintMenu(void){
  fprintf(stderr,
  "Usage: FALCON-map [OPTION]... [FILE] [FILE]                              \n"
  "Genomic similarity detection according to a reference                    \n"
  "                                                                         \n"
  "Non-mandatory arguments:                                                 \n"
  "                                                                         \n"
  "  -h                       give this help,                               \n"
  "  -V                       display version number,                       \n"
  "  -v                       verbose mode (more information),              \n"
  "  -s                       how compression levels,                       \n"
  "  -l <level>               compression level [1;30],                     \n"
  "  -t <top>                 top of similarity,                            \n"
  "  -n <nThreads>            number of threads,                            \n"
  "  -x <FILE>                similarity matrix filename,                   \n"
  "                                                                         \n"
  "Mandatory arguments:                                                     \n"
  "                                                                         \n"
  "  <FILE>                   sequenced file,                               \n"
  "  <FILE>                   base file (ex. from NCBI).                    \n"
  "                                                                         \n"
  "Report bugs to <{pratas,ap,pjf}@ua.pt>.                                \n");
  }

void PrintMenuVisual(void){
  fprintf(stderr,
  "Usage: FALCON-visual [OPTION]... [FILE]                                  \n"
  "A visual interpretation program for falcon-maps.                         \n"
  "                                                                         \n"
  "Non-mandatory arguments:                                                 \n"
  "                                                                         \n"
  "  -h                       give this help,                               \n"
  "  -V                       display version number,                       \n"
  "  -v                       verbose mode (more information),              \n"
  "  -w                       square width (for each value),                \n"
  "  -a                       square inter-space (between each value),      \n"
  "  -s                       index color start,                            \n"
  "  -r                       index color rotations,                        \n"
  "  -u                       color hue,                                    \n"
  "  -g                       color gamma,                                  \n"
  "  -x <FILE>                heatmap filename,                             \n"
  "                                                                         \n"
  "Mandatory arguments:                                                     \n"
  "                                                                         \n"
  "  <FILE>                   input matrix file (last argument).            \n"
  "                                                                         \n"
  "Report bugs to <{pratas,ap,pjf}@ua.pt>.                                \n");
  }

void PrintVersion(void){
  fprintf(stderr,
  "                                                                         \n"
  "                        ======================                           \n"
  "                        |   FALCON-map %u.%u   |                         \n"
  "                        ======================                           \n"
  "                                                                         \n"
  "FALCON-map: a genomic similarity detector according to a reference.      \n"
  "Copyright (C) 2014-2016 University of Aveiro. This is a Free software.   \n"
  "You may redistribute copies of it under the terms of the GNU - General   \n"
  "Public License v2 <http://www.gnu.org/licenses/gpl.html>. There is NOT   \n"
  "ANY WARRANTY, to the extent permitted by law. Developed and Written by   \n"
  "Diogo Pratas, Armando J. Pinho and Paulo J. S. G. Ferreira.\n\n", VERSION,
  RELEASE);
  }

void PrintVersionVisual(void){
  fprintf(stderr,
  "                                                                         \n"
  "                       =========================                         \n"
  "                       |   FALCON-visual %u.%u   |                       \n"
  "                       =========================                         \n"
  "                                                                         \n"
  "FALCON-visual: a visual interpretation program for falcon-maps.          \n"
  "Copyright (C) 2014-2016 University of Aveiro. This is a Free software.   \n"
  "You may redistribute copies of it under the terms of the GNU - General   \n"
  "Public License v2 <http://www.gnu.org/licenses/gpl.html>. There is NOT   \n"
  "ANY WARRANTY, to the extent permitted by law. Developed and Written by   \n"
  "Diogo Pratas, Armando J. Pinho and Paulo J. S. G. Ferreira.\n\n", VERSION,
  RELEASE);
  }


