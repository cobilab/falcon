#!/bin/bash
GET_FALCON=1;
GET_GOOSE=1;
GET_GULL=1;
GET_OTZI=1;
BUILD_OTZI=1;
BUILD_DB=1;
RUN_FALCON=1;
FILTER_GIS=1;
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
# GET GULL
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
if [[ "$GET_OTZI" -eq "1" ]]; then
  # ===========---------------------------------------------
  # | WARNING | THIS REQUIRES AT LEAST 300 GB OF FREE DISK |
  # ===========---------------------------------------------
  # http://www.ebi.ac.uk/ena/data/view/PRJEB2830
  #
  wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR069/ERR069107/ERR069107.fastq.gz
  wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR069/ERR069107/ERR069107_1.fastq.gz
  wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR069/ERR069107/ERR069107_2.fastq.gz
  #
  wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR069/ERR069108/ERR069108.fastq.gz
  wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR069/ERR069108/ERR069108_1.fastq.gz
  wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR069/ERR069108/ERR069108_2.fastq.gz
  #
  wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR069/ERR069109/ERR069109.fastq.gz
  wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR069/ERR069109/ERR069109_1.fastq.gz
  wget ftp://ftp.sra.ebi.ac.uk/vol1/fastq/ERR069/ERR069109/ERR069109_2.fastq.gz
fi
#==============================================================================
# BUILD SAMPLE
if [[ "$BUILD_OTZI" -eq "1" ]]; then
  zcat ERR069107.fastq.gz ERR069107_1.fastq.gz ERR069107_2.fastq.gz ERR069108.fastq.gz ERR069108_1.fastq.gz ERR069108_2.fastq.gz ERR069109.fastq.gz ERR069109_1.fastq.gz ERR069109_2.fastq.gz > OTZI
fi
#==============================================================================
# RUN FALCON
if [[ "$RUN_FALCON" -eq "1" ]]; then
  (time ./FALCON -v -n 8 -t 12000 -F -Z -m 20:100:1:5/10 -c 200 -y complexity.otzi OTZI DB.fa ) &> REPORT-FALCON-OTZI ;
  (time ./FALCON-FILTER -v -F -sl 0.001 -du 20000000 -t 0.5 -o positions.otzi complexity.otzi ) &> REPORT-FALCON-FILTER-OTZI ;
  (time ./FALCON-EYE -v -F  -e 500 -s 4 -sl 4.15 -o draw.otzi positions.otzi ) &> REPORT-FALCON-EYE-OTZI ;
  mv top.csv top-otzi.csv
fi
#==============================================================================
# RUN FILTER
if [[ "$FILTER_GIS" -eq "1" ]]; then
  cat top-otzi.csv | awk '{ if($3 > 0.001) print $1"\t"$2"\t"$3"\t"$4; }' \
  | awk '{ print $4;}' | tr '|' '\t' | awk '{ print $2;}' > GIS-OTZI;
  idx=0;
  cat GIS-OTZI | while read line
    do
    namex=`echo $line | tr ' ' '_'`;
    if [[ "$idx" -eq "0" ]]; then
      printf "%s" "$namex" > FNAMES-OTZI.fil;
      else
      printf ":%s" "$namex" >> FNAMES-OTZI.fil;
    fi
    ./goose-extractreadbypattern $line < DB.fa > $namex;
    ((idx++));
    done
  ./GULL-map -v -m 13:20:1:0/0 -m 20:100:1:5/10 -c 200 -n 8 -x MATRIX-OTZI.csv `cat FNAMES-OTZI.fil`
  ./GULL-visual -v -w 25 -a 8 -x HEATMAP-OTZI.svg MATRIX-OTZI.csv
fi
#==============================================================================

