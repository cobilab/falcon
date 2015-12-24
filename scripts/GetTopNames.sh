#!/bin/bash
# =============================================================================
# 
# GET THE MOST INFLUENT NAMES FROM TOP CACHE LIST GIVEN SCORE AND SIZE
# EDIT <THIS> IN LINE 8 : if($3 > <THIS>), FOR CUSTOM THRESHOLD 
#
# =============================================================================
SIZE=$1;
# =============================================================================
for((x=1 ; x<=$SIZE ; ++x));
  do
  rm -f UNIQ$x;
  cat TOP$x \
  | awk '{ if($3 > 85 && $2 > 400 && $3 != "-nan") 
        print $1"\t"$2"\t"$3"\t"$4"\n"; }' \
  | awk '{ print $4;}' \
  | tr '|' '\t' \
  | awk '{ print $5;} ' \
  | tr '_' '\t' \
  | awk '{ print $1" "$2;}' \
  | sort \
  | uniq -d \
  > UNIQ$x;
  done
# =============================================================================
rm -f ALL-UNIQ UNIQUE;
for((x=1 ; x<=$SIZE ; ++x));
  do
  cat UNIQ$x >> ALL-UNIQ;
  done
cat ALL-UNIQ | sort | uniq > UNIQUE;
# =============================================================================
