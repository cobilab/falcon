#!/bin/bash
GET_GOOSE=1;
GET_FALCON=1;
BUILD_DB=1;
RUN_FALCON=1;
#==============================================================================
# GET FALCON
if [[ "$GET_FALCON" -eq "1" ]]; then
  rm -fr falcon FALCON FALCON-* *.pl
  git clone https://github.com/pratas/falcon.git
  cd falcon/src/
  cmake .
  make
  cp FALCON ../../
  cp FALCON-FILTER ../../
  cp FALCON-EYE ../../
  cd ../../
  cp falcon/scripts/*.pl .
fi
#==============================================================================
# GET GOOSE
if [[ "$GET_GOOSE" -eq "1" ]]; then  
  rm -fr goose/ goose-*
  git clone https://github.com/pratas/goose.git
  cd goose/src/
  make
  cp goose-* ../../
  cd ../../
fi
#==============================================================================
# BUILD DB
if [[ "$BUILD_DB" -eq "1" ]]; then
  perl DownloadArchaea.pl
  perl DownloadBacteria.pl
  perl DownloadFungi.pl
  perl DownloadViruses.pl
  cat bacteria.fa | grep -v -e "ERROR" -e "eFetchResult" -e "DOCTYPE" -e "xml version" -e "Unable to obtain" | grep -v -x ">" > bacteria.fna
  mv bacteria.fna bacteria.fa
  cat viruses.fa bacteria.fa archaea.fa fungi.fa | tr ' ' '_' | ./goose-extractreadbypattern complete_genome | ./goose-getunique > DB.fa
fi
#==============================================================================
# RUN FALCON
if [[ "$RUN_FALCON" -eq "1" ]]; then
  (time ./FALCON -v -n 10 -t 10000 -F -Z -m 20:100:1:5/10 -c 200 -y complexity.com HNEAN DB.fa ) &> REPORT-FALCON ;
  (time ./FALCON-FILTER -v -F -sl 1 -du 20000000 -t 0.5 -o positions.pos complexity.com ) &> REPORT-FALCON-FILTER ;
  (time ./FALCON-EYE -v -F -o draw.map positions.pos ) &> REPORT-FALCON-EYE ;
fi
#==============================================================================
