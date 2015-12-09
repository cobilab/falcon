#ifndef LEVELS_H_INCLUDED
#define LEVELS_H_INCLUDED

#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// COMPRESSION LEVELS
//
// LEVEL_0 IS RESERVED TO THE ALGORITHM
#define LEVEL_1 "   1: -m 12:20:0:0/0 "
#define LEVEL_2 "   2: -m 12:20:1:0/0 "
#define LEVEL_3 "   3: -m 13:50:0:0/0 "
#define LEVEL_4 "   4: -m 13:50:1:0/0 "
#define LEVEL_5 "   5: -m 14:100:0:0/0 "
#define LEVEL_6 "   6: -m 14:100:1:0/0 "
#define LEVEL_7 "   7: -m 14:100:1:1/20 "
#define LEVEL_8 "   8: -m 18:200:0:1/50 -c 10 -g 0.9 "
#define LEVEL_9 "   9: -m 18:200:1:1/50 -c 10 -g 0.9 "
#define LEVEL_10 " 10: -m 18:200:1:1/50 -c 20 -g 0.9 "
#define LEVEL_11 " 11: -m 18:200:1:1/50 -c 30 -g 0.9 "
#define LEVEL_12 " 12: -m 18:200:1:1/50 -c 40 -g 0.9 "
#define LEVEL_13 " 13: -m 20:500:1:1/100 -c 10 -g 0.9 "
#define LEVEL_14 " 14: -m 20:500:1:1/100 -c 20 -g 0.9 "
#define LEVEL_15 " 15: -m 20:500:1:1/100 -c 30 -g 0.9 "
#define LEVEL_16 " 16: -m 20:500:1:1/100 -c 40 -g 0.9 "
#define LEVEL_17 " 17: -m 20:500:1:1/100 -c 50 -g 0.9 "
#define LEVEL_18 " 18: -m 20:500:1:2/50  -c 50 -g 0.9 "
#define LEVEL_19 " 19: -m 20:500:1:2/50  -m 13:100:1:0/0 -c 30 -g 0.9 "
#define LEVEL_20 " 20: -m 20:500:1:1/100 -m 13:100:1:0/0 -c 35 -g 0.9 "
#define LEVEL_21 " 21: -m 20:500:1:1/100 -m 13:100:1:0/0 -c 40 -g 0.9 "
#define LEVEL_22 " 22: -m 20:500:1:1/100 -m 13:100:1:0/0 -c 45 -g 0.9 "
#define LEVEL_23 " 23: -m 20:500:1:1/100 -m 14:100:1:0/0 -c 20 -g 0.9 "
#define LEVEL_24 " 24: -m 20:500:1:1/100 -m 14:100:1:0/0 -c 25 -g 0.9 "
#define LEVEL_25 " 25: -m 20:500:1:1/100 -m 14:100:1:0/0 -c 30 -g 0.9 "
#define LEVEL_26 " 26: -m 20:500:1:1/100 -m 14:100:1:0/0 -c 35 -g 0.9 "
#define LEVEL_27 " 27: -m 20:500:1:1/100 -m 14:100:1:0/0 -c 40 -g 0.9 "
#define LEVEL_28 " 28: -m 20:500:1:1/100 -m 14:100:1:0/0 -c 45 -g 0.9 "
#define LEVEL_29 " 29: -m 20:500:1:1/100 -m 14:100:1:0/0 -c 50 -g 0.9 "
#define LEVEL_30 " 30: -m 20:500:1:1/100 -m 14:100:1:0/0 -c 60 -g 0.9 "
#define LEVEL_31 " 31: -m 20:500:1:1/100 -m 14:100:1:0/0 -c 70 -g 0.9 "
#define LEVEL_32 " 32: -m 20:500:1:1/100 -m 14:100:1:0/0 -m 12:1:0:0/0 -m 8:1:0:0/0 -c 30 "
#define LEVEL_33 " 33: -m 20:500:1:1/100 -m 14:100:1:0/0 -m 12:1:0:0/0 -m 8:1:0:0/0 -c 40 "
#define LEVEL_34 " 34: -m 20:500:1:1/100 -m 14:100:1:0/0 -m 12:1:0:0/0 -m 8:1:0:0/0 -c 50 "
#define LEVEL_35 " 35: -m 20:500:1:1/100 -m 14:100:1:0/0 -m 12:1:0:0/0 -m 8:1:0:0/0 -m 4:1:0:0/0 -c 50 "
#define LEVEL_36 " 36: -m 6:1:0:0/0 "
#define LEVEL_37 " 37: -m 11:1:1:0/0 "
#define LEVEL_38 " 38: -m 11:1:0:0/0 "
#define LEVEL_39 " 39: -m 14:50:1:0/0 "
#define LEVEL_40 " 40: -m 14:50:1:1/10 "
#define LEVEL_41 " 41: -m 8:1:0:0/0 -m 14:50:1:0/0 "
#define LEVEL_42 " 42: -m 4:1:0:0/0 -m 14:50:1:0/0 "
#define LEVEL_43 " 43: -m 6:1:0:0/0 -m 14:50:1:0/0 "
#define LEVEL_44 " 44: -m 11:1:0:0/0 -m 14:50:1:0/0 "

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char    *GetLevels  (uint8_t);
void    PrintLevels (void);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

