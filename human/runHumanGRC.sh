#!/bin/bash
GET_FALCON=1;
GET_GOOSE=1;
GET_GULL=1;
GET_GRC=1;
BUILD_DB=1;
RUN_FALCON=1;
FILTER_GIS=1;
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
  cp falcon/scripts/GetHumanGRCParse.sh .
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
# GET GOOSE
if [[ "$GET_GULL" -eq "1" ]]; then
  rm -fr GULL/ GULL-map GULL-visual
  git clone https://github.com/pratas/GULL.git
  cd GULL/src/
  cmake .
  make
  cp GULL-map ../../
  cp GULL-visual ../../
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
if [[ "$GET_GRC" -eq "1" ]]; then
  # FALCON INSTALL DEPENDENCY
  . GetHumanGRCParse.sh
fi
#==============================================================================
# RUN FALCON
if [[ "$RUN_FALCON" -eq "1" ]]; then
  (time ./FALCON -v -n 8 -t 12000 -F -Z -m 20:100:1:5/10 -c 200 -y complexity.grc GRC DB.fa ) &> REPORT-FALCON-GRC ;
  (time ./FALCON-FILTER -v -F -sl 0.001 -du 20000000 -t 0.5 -o positions.grc complexity.grc ) &> REPORT-FALCON-FILTER-GRC ;
  (time ./FALCON-EYE -v -e 500 -s 4 -F -o draw.svg positions.grc ) &> REPORT-FALCON-EYE-GRC ;
fi
#==============================================================================
# RUN FILTER
if [[ "$FILTER_GIS" -eq "1" ]]; then
  cat top.csv | awk '{ if($3 > 0.001) print $1"\t"$2"\t"$3"\t"$4; }' \
  | awk '{ print $4;}' | tr '|' '\t' | awk '{ print $2;}' > GIS;
  idx=0;
  cat GIS | while read line
    do
    namex=`echo $line | tr ' ' '_'`;
    if [[ "$idx" -eq "0" ]]; then
      printf "%s" "$namex" > FNAMES.fil;
      else
      printf ":%s" "$namex" >> FNAMES.fil;
    fi
    ./goose-extractreadbypattern $line < DB.fa > $namex;
    ((idx++));
    done
  ./GULL-map -v -m 20:100:1:5/10 -c 30 -n 8 -x MATRIX.csv `cat FNAMES.fil`
  ./GULL-visual -v -w 25 -a 8 -x HEATMAP.svg MATRIX.csv
fi
#==============================================================================
