#!/bin/bash
# =============================================================================
# 
# GET THE MOST INFLUENT NAMES FROM TOP CACHE LIST GIVEN SCORE AND SIZE
# EDIT <THIS> IN LINE 8 : if($3 > <THIS>), FOR CUSTOM THRESHOLD 
#
# =============================================================================
for((x=1 ; x<=56 ; ++x));
  do
  rm -f UNIQ-NEAN$x;
  cat TOP-NEAN$x \
  | awk '{ if($3 > 85 && $2 > 400) 
        print $1"\t"$2"\t"$3"\t"$4"\n"; }' \
  | awk '{ print $4;}' \
  | tr '|' '\t' \
  | awk '{ print $5;} ' \
  | tr '_' '\t' \
  | awk '{ print $1" "$2;}' \
  | sort \
  | uniq -d \
  > UNIQ-NEAN$x;
  done
# =============================================================================
rm -f ALL-UNIQ-NEAN UNIQUE-NEAN;
for((x=1 ; x<=56 ; ++x));
  do
  cat UNIQ-NEAN$x >> ALL-UNIQ-NEAN;
  done
cat ALL-UNIQ-NEAN | sort | uniq > UNIQUE-NEAN;
# =============================================================================
