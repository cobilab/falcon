#!/bin/bash
GET_GOOSE=1;
GET_FALCON=1;
GET_DENISOVAN=1;
BUILD_DB=1;
RUN_FALCON=1;
#==============================================================================
# sudo apt-get install git cmake
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
if [[ "$GET_DENISOVAN" -eq "1" ]]; then
  # ===========----------------------------------
  # | WARNING | THIS REQUIRES 1 TB OF FREE DISK |
  # ===========----------------------------------
  EVANM="http://cdna.eva.mpg.de";
  EVAPT="$EVANM/denisova/raw_reads";
  WGETO=" --trust-server-names -q ";
  echo "Downloading sequences ... (This may take a while!)";
  wget $WGETO $EVAPT/B1087_SR.txt.gz -O DX1.gz
  wget $WGETO $EVAPT/B1088_SR.txt.gz -O DX2.gz
  wget $WGETO $EVAPT/B1101_SR.txt.gz -O DX3.gz
  wget $WGETO $EVAPT/B1102_SR.txt.gz -O DX4.gz
  wget $WGETO $EVAPT/B1107_SR.txt.gz -O DX5.gz
  wget $WGETO $EVAPT/B1108_SR.txt.gz -O DX6.gz
  wget $WGETO $EVAPT/B1109_SR.txt.gz -O DX7.gz
  wget $WGETO $EVAPT/B1110_SR.txt.gz -O DX8.gz
  wget $WGETO $EVAPT/B1128_SR.txt.gz -O DX9.gz
  wget $WGETO $EVAPT/B1130_SR.txt.gz -O DX10.gz
  wget $WGETO $EVAPT/B1133_SR.txt.gz -O DX11.gz
  wget $WGETO $EVAPT/SL3003_SR.txt.gz -O DX12.gz
  wget $WGETO $EVAPT/SL3004_SR.txt.gz -O DX13.gz
  for((x=1 ; x<=13 ; ++x));
    do
    gunzip DX$x.gz
    done
  rm -f DENIS;
  cat DX* >> DENIS;
  rm -f DX*;
fi
#==============================================================================
# RUN FALCON
if [[ "$RUN_FALCON" -eq "1" ]]; then
  (time ./FALCON -v -n 8 -t 12000 -F -Z -m 20:100:1:5/10 -c 200 -y complexity.denis DENIS DB.fa ) &> REPORT-FALCON ;
  (time ./FALCON-FILTER -v -F -sl 1 -du 20000000 -t 0.5 -o positions.denis complexity.denis ) &> REPORT-FALCON-FILTER ;
  (time ./FALCON-EYE -v -F -o draw.map positions.denis ) &> REPORT-FALCON-EYE ;
fi
#==============================================================================
