#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "levels.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GET PARAMETERS FROM LEVELS
//
char *GetLevels(uint8_t l){
  switch(l){
    case 1:  return LEVEL_1;
    case 2:  return LEVEL_2;
    case 3:  return LEVEL_3;
    case 4:  return LEVEL_4;
    case 5:  return LEVEL_5;
    case 6:  return LEVEL_6;
    case 7:  return LEVEL_7;
    case 8:  return LEVEL_8;
    case 9:  return LEVEL_9;
    case 10: return LEVEL_10;
    case 11: return LEVEL_11;
    case 12: return LEVEL_12;
    case 13: return LEVEL_13;
    case 14: return LEVEL_14;
    case 15: return LEVEL_15;
    case 16: return LEVEL_16;
    case 17: return LEVEL_17;
    case 18: return LEVEL_18;
    case 19: return LEVEL_19;
    case 20: return LEVEL_20;
    case 21: return LEVEL_21;
    case 22: return LEVEL_22;
    case 23: return LEVEL_23;
    case 24: return LEVEL_24;
    case 25: return LEVEL_25;
    case 26: return LEVEL_26;
    case 27: return LEVEL_27;
    case 28: return LEVEL_28;
    case 29: return LEVEL_29;
    case 30: return LEVEL_30;
    case 31: return LEVEL_31;
    case 32: return LEVEL_32;
    default: fprintf(stderr, "[x] Unknown level!\n");
    exit(1);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// PRINT LEVELS FROM LEVELS
//
void PrintLevels(void){
  fprintf(stderr, "Level%s\n", LEVEL_1);
  fprintf(stderr, "Level%s\n", LEVEL_2);
  fprintf(stderr, "Level%s\n", LEVEL_3);
  fprintf(stderr, "Level%s\n", LEVEL_4);
  fprintf(stderr, "Level%s\n", LEVEL_5);
  fprintf(stderr, "Level%s\n", LEVEL_6);
  fprintf(stderr, "Level%s\n", LEVEL_7);
  fprintf(stderr, "Level%s\n", LEVEL_8);
  fprintf(stderr, "Level%s\n", LEVEL_9);
  fprintf(stderr, "Level%s\n", LEVEL_10);
  fprintf(stderr, "Level%s\n", LEVEL_11);
  fprintf(stderr, "Level%s\n", LEVEL_12);
  fprintf(stderr, "Level%s\n", LEVEL_13);
  fprintf(stderr, "Level%s\n", LEVEL_14);
  fprintf(stderr, "Level%s\n", LEVEL_15);
  fprintf(stderr, "Level%s\n", LEVEL_16);
  fprintf(stderr, "Level%s\n", LEVEL_17);
  fprintf(stderr, "Level%s\n", LEVEL_18);
  fprintf(stderr, "Level%s\n", LEVEL_19);
  fprintf(stderr, "Level%s\n", LEVEL_20);
  fprintf(stderr, "Level%s\n", LEVEL_21);
  fprintf(stderr, "Level%s\n", LEVEL_22);
  fprintf(stderr, "Level%s\n", LEVEL_23);
  fprintf(stderr, "Level%s\n", LEVEL_24);
  fprintf(stderr, "Level%s\n", LEVEL_25);
  fprintf(stderr, "Level%s\n", LEVEL_26);
  fprintf(stderr, "Level%s\n", LEVEL_27);
  fprintf(stderr, "Level%s\n", LEVEL_28);
  fprintf(stderr, "Level%s\n", LEVEL_29);
  fprintf(stderr, "Level%s\n", LEVEL_30);
  fprintf(stderr, "Level%s\n", LEVEL_31);
  fprintf(stderr, "Level%s\n", LEVEL_32);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

