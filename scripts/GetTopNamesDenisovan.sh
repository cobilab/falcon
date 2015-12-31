#!/bin/bash
# =============================================================================
# 
# GET THE MOST INFLUENT NAMES FROM TOP CACHE LIST GIVEN SCORE AND SIZE
# EDIT <THIS> IN LINE 8 : if($3 > <THIS>), FOR CUSTOM THRESHOLD 
#
# =============================================================================
for((x=1 ; x<=13 ; ++x));
  do
  rm -f UNIQ-DENI$x;
  cat TOP-DENI$x \
  | awk '{ if($3 > 85 && $2 > 400) 
        print $1"\t"$2"\t"$3"\t"$4"\n"; }' \
  | awk '{ print $4;}' \
  | tr '|' '\t' \
  | awk '{ print $5;} ' \
  | tr '_' '\t' \
  | awk '{ print $1" "$2;}' \
  | sort \
  | uniq -d \
  > UNIQ-DENI$x;
  done
# =============================================================================
rm -f ALL-UNIQ-DENI UNIQUE-DENI;
for((x=1 ; x<=13 ; ++x));
  do
  cat UNIQ-DENI$x >> ALL-UNIQ-DENI;
  done
cat ALL-UNIQ-DENI | sort | uniq > UNIQUE-DENI;
# =============================================================================
