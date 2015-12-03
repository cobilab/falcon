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
  "Usage: FALCON [OPTION]... [FILE] [FILE]                                  \n"
  "Measure a top similarity between genomic sequences.                      \n"
  "                                                                         \n"
  "Non-mandatory arguments:                                                 \n"
  "                                                                         \n"
  "  -h                       give this help,                               \n"
  "  -V                       display version number,                       \n"
  "  -v                       verbose mode (more information),              \n"
  "  -s                       show compression levels,                      \n"
  "  -l <level>               compression level [1;30],                     \n"
  "  -p <sample>              subsampling (default: 1),                     \n"
  "  -t <top>                 top of similarity,                            \n"
  "  -n <nThreads>            number of threads,                            \n"
  "  -x <FILE>                similarity top filename,                      \n"
  "                                                                         \n"
  "Mandatory arguments:                                                     \n"
  "                                                                         \n"
  "  [FILE]                   sequenced file (FASTA or FASTQ),              \n"
  "  [FILE]                   base file (FASTA or Multi-FASTA).             \n"
  "                                                                         \n"
  "Report issues to <{pratas,ap,pjf,jmr}@ua.pt>.                            \n");
  }

void PrintVersion(void){
  fprintf(stderr,
  "                                                                         \n"
  "                          ==================                             \n"
  "                          |   FALCON %u.%u   |                           \n"
  "                          ==================                             \n"
  "                                                                         \n"
  "FALCON: a tool for measure a top similarity between genomic sequences.   \n"
  "Copyright (C) 2014-2016 University of Aveiro. This is a Free software.   \n"
  "You may redistribute copies of it under the terms of the GNU - General   \n"
  "Public License v2 <http://www.gnu.org/licenses/gpl.html>. There is NOT   \n"
  "ANY WARRANTY, to the extent permitted by law. Developed and Written by   \n"
  "Diogo Pratas, Armando J. Pinho, Paulo J. S. G. Ferreira and Joao M. O.   \n"
  "S. Rodrigues.\n\n", VERSION, RELEASE);
  }

