#include "msg.h"
#include <stdio.h>
#include <stdlib.h>

void ModelsExplanation(void){
  fprintf(stderr,
  "                                                                         \n"
  "      -m <c>:<d>:<i>:<m/e>  reference model (ex: -m 13:20:1:0/0),        \n"
  "      -m <c>:<d>:<i>:<m/e>  reference model (ex: -m 18:500:1:5/50),      \n"
  "      ...                                                                \n"
  "                 templates use <c> for context-order size, <d> for       \n"
  "                 alpha (1/<d>), <i> (0 or 1) to set the usage of         \n"
  "                 inverted repeats (set 1 to use) and <m> to the          \n"
  "                 maximum allowed mutation on the context without         \n"
  "                 being discarded (usefull in deep contexts), under       \n"
  "                 the estimator <e>.                                      \n"
  "                                                                         \n");
  }

void PrintMenu(void){
  fprintf(stderr,
  "                                                                         \n"
  "NAME                                                                     \n"
  "      FALCON v%u.%u - A tool to infer metagenomic composition.           \n"
  "                                                                         \n"
  "AUTHORS                                                                  \n"
  "      Diogo Pratas        pratas@ua.pt                                   \n"
  "      Armando J. Pinho    ap@ua.pt                                       \n"
  "                                                                         \n"
  "SYNOPSIS                                                                 \n"
  "      ./FALCON [OPTION]... [FILE_1] [FILE_2]                             \n"
  "                                                                         \n"
  "SAMPLE                                                                   \n"
  "      ./FALCON -v -l 47 -t 20 reads.fq database.fa                       \n"
  "                                                                         \n"
  "DESCRIPTION                                                              \n"
  "      It infers metagenomic sample composition of sequenced reads.       \n"
  "      The reference sequences must be in a multi-FASTA format.           \n"
  "      The sequences reads must be trimmed and in FASTQ format.           \n"
  "                                                                         \n"
  "Non-mandatory arguments:                                                 \n"
  "                                                                         \n"
  "      -h                give this help,                                  \n"
  "      -F                force mode (overwrites top file),                \n"
  "      -V                display version number,                          \n"
  "      -v                verbose mode (more information),                 \n"
  #ifdef LOCAL_SIMILARITY
  "      -Z                database local similarity,                       \n"
  #endif
  "      -s                show compression levels,                         \n"
  "      -l <level>        compression level [%u;%u],                       \n"
  "      -p <sample>       subsampling (default: %u),                       \n"
  "      -t <top>          top of similarity (default: %u),                 \n"
  "      -n <nThreads>     number of threads (default: %u),                 \n"
  "      -x <FILE>         similarity top filename,                         \n"
  #ifdef LOCAL_SIMILARITY
  "      -y <FILE>         local similarities filename,                     \n"
  #endif
  "                                                                         \n"
  "Mandatory arguments:                                                     \n"
  "                                                                         \n"
  "      [FILE1]           metagenomic filename (FASTA or FASTQ),           \n"
  "      [FILE2]           database filename (FASTA or Multi-FASTA).        \n"
  "                                                                         \n"
  "COPYRIGHT                                                                \n"
  "      Copyright (C) 2014-2019, IEETA, University of Aveiro.              \n"
  "      This is a Free software, under GPLv3. You may redistribute         \n"
  "      copies of it under the terms of the GNU - General Public           \n"
  "      License v3 <http://www.gnu.org/licenses/gpl.html>. There           \n"
  "      is NOT ANY WARRANTY, to the extent permitted by law.               \n"
  "                                                                         \n",
  VERSION, RELEASE, (uint32_t) MIN_LEV, (uint32_t) MAX_LEV, (uint32_t) 
  DEFAULT_SAMPLE, (uint32_t) DEF_TOP, (uint32_t) DEFAULT_THREADS);
  }

void PrintMenuFilter(void){
  fprintf(stderr,
  "Usage: FALCON-FILTER [OPTION]... [FILE]                                  \n"
  "Filter and segment FALCON output.                                        \n"
  "                                                                         \n"
  "Non-mandatory arguments:                                                 \n"
  "                                                                         \n"
  "  -h                       give this help,                               \n"
  "  -F                       force mode (overwrites top file),             \n"
  "  -V                       display version number,                       \n"
  "  -v                       verbose mode (more information),              \n"
  "  -s  <size>               filter window size,                           \n"
  "  -w  <type>               filter window type,                           \n"
  "  -x  <sampling>           filter window sampling,                       \n"
  "  -sl <lower>              similarity lower bound,                       \n"
  "  -su <upper>              similarity upper bound,                       \n"
  "  -dl <lower>              size lower bound,                             \n"
  "  -du <upper>              size upper bound,                             \n"
  "  -t  <threshold>          threshold [0;2.0],                            \n"
  "  -o  <FILE>               output filename,                              \n"
  "                                                                         \n"
  "Mandatory arguments:                                                     \n"
  "                                                                         \n"
  "  [FILE]                   profile filename (from FALCON),               \n"
  "                                                                         \n"
  "COPYRIGHT                                                                \n"
  "    Copyright (C) 2014-2019, IEETA, University of Aveiro.                \n"
  "    This is a Free software, under GPLv3. You may redistribute           \n"
  "    copies of it under the terms of the GNU - General Public             \n"
  "    License v3 <http://www.gnu.org/licenses/gpl.html>. There             \n"
  "    is NOT ANY WARRANTY, to the extent permitted by law.                 \n");
  }

void PrintMenuEye(void){
  fprintf(stderr,
  "Usage: FALCON-EYE [OPTION]... [FILE]                                     \n"
  "Visualize FALCON-FILTER output.                                          \n"
  "                                                                         \n"
  "Non-mandatory arguments:                                                 \n"
  "                                                                         \n"
  "  -h                       give this help,                               \n"
  "  -F                       force mode (overwrites top file),             \n"
  "  -V                       display version number,                       \n"
  "  -v                       verbose mode (more information),              \n"
  "  -w  <width>              square width (for each value),                \n"
  "  -s  <ispace>             square inter-space (between each value),      \n"
  "  -i  <indexs>             color index start,                            \n"
  "  -r  <indexr>             color index rotations,                        \n"
  "  -u  <hue>                color hue,                                    \n"
  "  -sl <lower>              similarity lower bound,                       \n"
  "  -su <upper>              similarity upper bound,                       \n"
  "  -dl <lower>              size lower bound,                             \n"
  "  -du <upper>              size upper bound,                             \n"
  "  -bg                      show only the best of group,                  \n"
  "  -g  <color>              color gamma,                                  \n"
  "  -e  <size>               enlarge painted regions,                      \n"
  "  -ss                      do NOT show global scale,                     \n"
  "  -sn                      do NOT show names,                            \n"
  "  -o  <FILE>               output image filename,                        \n"
  "                                                                         \n"
  "Mandatory arguments:                                                     \n"
  "                                                                         \n"
  "  [FILE]                   profile filename (from FALCON-FILTER),        \n"
  "                                                                         \n"
  "COPYRIGHT                                                                \n"
  "    Copyright (C) 2014-2019, IEETA, University of Aveiro.                \n"
  "    This is a Free software, under GPLv3. You may redistribute           \n"
  "    copies of it under the terms of the GNU - General Public             \n"
  "    License v3 <http://www.gnu.org/licenses/gpl.html>. There             \n"
  "    is NOT ANY WARRANTY, to the extent permitted by law.                 \n");
  }

void PrintVersion(void){
  fprintf(stderr,
  "                                                                         \n"
  "                                                                         \n"
  "           ███████╗ █████╗ ██╗      ██████╗ ██████╗ ███╗   ██╗           \n"
  "           ██╔════╝██╔══██╗██║     ██╔════╝██╔═══██╗████╗  ██║           \n"
  "           █████╗  ███████║██║     ██║     ██║   ██║██╔██╗ ██║           \n"
  "           ██╔══╝  ██╔══██║██║     ██║     ██║   ██║██║╚██╗██║           \n"
  "           ██║     ██║  ██║███████╗╚██████╗╚██████╔╝██║ ╚████║           \n"
  "           ╚═╝     ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝           \n"
  "                                                                         \n"
  "                          ==================                             \n"
  "                          |   FALCON %u.%u   |                           \n"
  "                          ==================                             \n"
  "                                                                         \n"
  "           An ultra-fast tool to infer metagenomic composition.          \n"
  "                                                                         \n"
  "              Copyright (C) 2014-2019 University of Aveiro.              \n"
  "                                                                         \n"
  "                  This is a Free software, under GPLv3.                  \n"
  "                                                                         \n"
  "You may redistribute copies of it under the terms of the GNU - General   \n"
  "Public License v3 <http://www.gnu.org/licenses/gpl.html>. There is NOT   \n"
  "ANY WARRANTY, to the extent permitted by law.\n\n", VERSION, RELEASE);
  }

