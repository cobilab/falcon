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
  "                                                                         \n");
  }

void PrintMenu(void){
  fprintf(stderr,
  "Usage: FALCON [OPTION]... [FILE1] [FILE2]                                \n"
  "Machine learning system to detect pathogens in metagenomic samples.      \n"
  "                                                                         \n"
  "Non-mandatory arguments:                                                 \n"
  "                                                                         \n"
  "  -h                       give this help,                               \n"
  "  -F                       force mode (overwrites top file),             \n"
  "  -V                       display version number,                       \n"
  "  -v                       verbose mode (more information),              \n"
  #ifdef LOCAL_SIMILARITY
  "  -Z                       database local similarity,                    \n"
  #endif
  "  -s                       show compression levels,                      \n"
  "  -l <level>               compression level [%u;%u],                    \n"
  "  -p <sample>              subsampling (default: %u),                    \n"
  "  -t <top>                 top of similarity (default: %u),              \n"
  "  -n <nThreads>            number of threads (default: %u),              \n"
  "  -x <FILE>                similarity top filename,                      \n"
  #ifdef LOCAL_SIMILARITY
  "  -y <FILE>                local similarities filename,                  \n"
  #endif
  "                                                                         \n"
  "Mandatory arguments:                                                     \n"
  "                                                                         \n"
  "  [FILE1]                  metagenomic filename (FASTA or FASTQ),        \n"
  "  [FILE2]                  database filename (FASTA or Multi-FASTA).     \n"
  "                                                                         \n"
  "Report issues to <{pratas,ap,pjf,jmr}@ua.pt>.                            \n",
  MIN_LEV, MAX_LEV, DEFAULT_SAMPLE, DEF_TOP, DEFAULT_THREADS);
  }

void PrintMenuEye(void){
  fprintf(stderr,
  "Usage: FALCON-EYE [OPTION]... [FILE]                                     \n"
  "Vizualise FALCON output: filter, segment and image drawer.               \n"
  "                                                                         \n"
  "Non-mandatory arguments:                                                 \n"
  "                                                                         \n"
  "  -h                       give this help,                               \n"
  "  -F                       force mode (overwrites top file),             \n"
  "  -V                       display version number,                       \n"
  "  -v                       verbose mode (more information),              \n"
  "  -w                       square width (for each value),                \n"
  "  -a                       square inter-space (between each value),      \n"
  "  -s                       color index start,                            \n"
  "  -r                       color index rotations,                        \n"
  "  -u                       color hue,                                    \n"
  "  -g                       color gamma,                                  \n"
  "  -o <FILE>                output image filename,                        \n"
  "                                                                         \n"
  "Mandatory arguments:                                                     \n"
  "                                                                         \n"
  "  [FILE]                   profile filename (from FALCON),               \n"
  "                                                                         \n"
  "Report issues to <{pratas,ap,pjf,jmr}@ua.pt>.                           \n");
  }

void PrintVersion(void){
  fprintf(stderr,
  "                                                                         \n"
  "                          ==================                             \n"
  "                          |   FALCON %u.%u   |                           \n"
  "                          ==================                             \n"
  "                                                                         \n"
  "Machine learning system that detects pathogens in metagenomic samples.   \n"
  "Copyright (C) 2014-2016 University of Aveiro. This is a Free software.   \n"
  "You may redistribute copies of it under the terms of the GNU - General   \n"
  "Public License v3 <http://www.gnu.org/licenses/gpl.html>. There is NOT   \n"
  "ANY WARRANTY, to the extent permitted by law. Developed and Written by   \n"
  "Diogo Pratas, Armando J. Pinho, Paulo J. S. G. Ferreira and Joao M. O.   \n"
  "S. Rodrigues.\n\n", VERSION, RELEASE);
  }

