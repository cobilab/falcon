#!/bin/bash
###############################################################################
# PERFORMANCE OF FALCON HANDLING INCREASING RANDOM PERMUTATION OF BLOCK SIZES
# REQUIREMENTS: linux OS with cmake, git and gnuplot
# (sudo apt-get install cmake git gnuplot)
###############################################################################
# PARAMETERS ==================================================================
RENEW=1;
INSTALL=1;
RUN=1;
PLOT=1;
#==============================================================================
FQLINE=100;
FQNREADS_MIN=200;
FQNREADS_MAX=4000;
FQNREADS_STEP=200;
PMIN=30;   #PERMUTATION MINIMUM
PMAX=250;  #PERMUTATION MAXIMUM
JUMP=1;    #JUMP BETWEEN LOOPS
#==============================================================================
DISTRIBUTION="0.3,0.2,0.2,0.3,0.000";
EXTRAMUT=" ";
#EXTRAMUT=" -ir 0.01 -dr 0.01 ";
#FPARAM=" -m 20:500:1:10/20 -m 14:100:1:0/0 -m 12:1:0:0/0 -m 4:1:0:0/0 \
#-c 20 -g 0.95 ";
#GPARAM=" -rm 20:500:1:10/20 -rm 14:100:1:0/0 -rm 12:1:0:0/0 -rm 4:1:0:0/0 \
#-c 15 -g 0.95 ";
FPARAM=" -m 20:1000:0:5/50 -c 20 -g 0.95 ";
GPARAM=" -rm 20:1000:1:5/50 -c 20 -g 0.95 ";
GREENPARAM=" -i -f 10 -k 15 ";
###############################################################################
if [[ "$RENEW" -eq "1" ]]; then
  rm -fr ttmp;
  mkdir ttmp;
fi
cd ttmp/
###############################################################################
if [[ "$INSTALL" -eq "1" ]]; then
# CLEAN & INSTALL =============================================================
rm -fr goose-* FALCON XS goose/ falcon/ xs/ geco/ SAMPLE* DB-mfa;
# GET GOOSE FRAMEWORK =========================================================
git clone https://github.com/pratas/goose.git
cd goose/src/
make
cd ../../
cp goose/src/goose-* .
cp goose/scripts/ShufFASTQReads.sh .
cp goose/scripts/GlobalMUMmer.sh .
# GET FALCON ==================================================================
git clone https://github.com/pratas/falcon.git
cd falcon/src/
cmake .
make
cp FALCON ../../
cd ../../
# GET XS ======================================================================
git clone https://github.com/pratas/xs.git
cd xs
make
cp XS ../
cd ..
# GET GREEN ===================================================================
wget http://bioinformatics.ua.pt/wp-content/uploads/2014/09/GReEn1.tar.gz
tar -xvzf GReEn1.tar.gz
cd GReEn1/
make
cp GReEnC ../
cd ..
# GET MUMmer 3.23 =============================================================
rm -rf MUMmer.tar.gz MUMmer3.23/
wget -O MUMmer.tar.gz \
http://downloads.sourceforge.net/project/mummer/mummer/3.23/MUMmer3.23.tar.gz
tar -xvzf MUMmer.tar.gz
cd MUMmer3.23
make check
make install
cd ..
cp MUMmer3.23/nucmer .
cp MUMmer3.23/show-coords .
cp MUMmer3.23/delta-filter .
fi
###############################################################################
# RUN  ========================================================================
if [[ "$RUN" -eq "1" ]]; then
rm -f TIME_FALCON TIME_GREEN TIME_MUMMER TIME_MUMMER20;
for((y=$FQNREADS_MIN ; y<=$FQNREADS_MAX ; y+=$FQNREADS_STEP));
  do
  ./XS -v -ls $FQLINE -n $y -f $DISTRIBUTION -s 0 SAMPLE.fq
  # MUTATE ====================================================================
  ./goose-fastq2fasta < SAMPLE.fq > SAMPLE.fa
  ./goose-fasta2seq   < SAMPLE.fa > SAMPLE
  ./goose-seq2fasta -n "Permutation0" < SAMPLE > SAMPLE0.fa
  for((x=$PMIN ; x<=$PMAX ; x+=$JUMP));
    do
    echo "Permutation rate: $x";
    ./goose-permuteseqbyblocks -bs $x -s 101 < SAMPLE > SAMPLE$x
    ./goose-seq2fasta -n "Permutation$x" < SAMPLE$x > SAMPLE$x.fa
    cat SAMPLE$x.fa >> DB.mfa;
    done
  . ShufFASTQReads.sh SAMPLE.fq > SAMPLE-SHUF.fq
  mv SAMPLE-SHUF.fq SAMPLE.fq
  ./goose-fastq2fasta < SAMPLE.fq > SAMPLE.fa
  # RUN FALCON ================================================================
  START=$(date +%s.%N)
  ./FALCON -v -F $FPARAM -n 4 -t $PMAX -x TOP-SUBS SAMPLE.fq DB.mfa
  cat TOP-SUBS | awk '{ print $4"\t"$3;}' | sed 's/\Permutation//g' | sort -n \
  | awk '{ print $1"\t"$2;}' > TOP-SUBS-FILT;
  END=$(date +%s.%N)
  DIFF=$(echo "$END - $START" | bc)
  echo "$y $DIFF" >> TIME_FALCON
  # RUN GREEN =================================================================
  rm -f TOP-GREEN;
  cat SAMPLE.fa | grep -v ">" | tr -d -c "ACGTN" > SAMPLE.seq;
  START=$(date +%s.%N)
  GSIZE=`./goose-info < SAMPLE.seq  | grep "Number of" | awk '{ print $4;}'`;
  for((x=$PMIN ; x<=$PMAX ; x+=$JUMP));
    do
    printf "%u\t" "$x" >> TOP-GREEN;
    cat SAMPLE$x.fa | grep -v ">" | tr -d -c "ACGTN" > SAMPLEX.seq;
    ./GReEnC -v $GREENPARAM SAMPLE.seq SAMPLEX.seq > TMPX;
    GVALUE=`cat TMPX | grep "number of bytes" | awk '{ print $5;}'`;
    GRES=`echo "scale=6;100-((8*($GVALUE)/(2*$GSIZE))*100)" | bc -l`;
    echo $GRES >> TOP-GREEN;
    done
  END=$(date +%s.%N)
  DIFF=$(echo "$END - $START" | bc)
  echo "$y $DIFF" >> TIME_GREEN
  # RUN MUMMER ================================================================
  rm -f TOP-MUMMER;
  START=$(date +%s.%N)
  for((x=$PMIN ; x<=$PMAX ; x+=$JUMP));
    do
    printf "%u\t" "$x" >> TOP-MUMMER;
    ./nucmer -p mummer-tmp SAMPLE.fa SAMPLE$x.fa
    ./delta-filter -1 mummer-tmp.delta > mummer-tmp.delta2
    ./show-coords -clr mummer-tmp.delta2 > mummer-tmp.delta3
    echo "Running Global similarity for MUMmer ...";
    . GlobalMUMmer.sh mummer-tmp.delta3 >> TOP-MUMMER;
    done
  END=$(date +%s.%N)
  DIFF=$(echo "$END - $START" | bc)
  echo "$y $DIFF" >> TIME_MUMMER
  # RUN MUMMER 20 =============================================================
  rm -f TOP-MUMMER20;
  START=$(date +%s.%N)
  for((x=$PMIN ; x<=$PMAX ; x+=$JUMP));
    do
    printf "%u\t" "$x" >> TOP-MUMMER20;
    ./nucmer -c 20 -p mummer-tmp SAMPLE.fa SAMPLE$x.fa
    ./delta-filter -1 mummer-tmp.delta > mummer-tmp.delta2
    ./show-coords -clr mummer-tmp.delta2 > mummer-tmp.delta3
    echo "Running Global similarity for MUMmer -c 20 ...";
    . GlobalMUMmer.sh mummer-tmp.delta3 >> TOP-MUMMER20;
    done
  END=$(date +%s.%N)
  DIFF=$(echo "$END - $START" | bc)
  echo "$y $DIFF" >> TIME_MUMMER20
  done
fi
###############################################################################
# PLOT ========================================================================
if [[ "$PLOT" -eq "1" ]]; then
gnuplot << EOF
set terminal pdfcairo enhanced color font 'Verdana,14'
set output "time.pdf"
set auto
set size ratio 0.75
set key outside right center
unset grid
set ylabel "Time (s)"
set xlabel "Number of reads"
set style line 1 lt 2 lc rgb "#CA000D" lw 3
set style line 2 lt 2 lc rgb "#018D0F" lw 3
set style line 3 lt 2 lc rgb "#000DC0" lw 3
set style line 4 lt 2 lc rgb "#FF811F" lw 3
set style line 5 lt 2 lc rgb "black" lw 3
plot [$FQNREADS_MIN:$FQNREADS_MAX] "TIME_MUMMER" u 1:2 w lines ls 3 title "MUMmer", \
 "TIME_MUMMER20" u 1:2 w lines ls 4 title "MUMmer -c 20", \
 "TIME_GREEN" u 1:2 w lines ls 2 title "GREEN", \
 "TIME_FALCON" u 1:2 w lines ls 1 title "FALCON"
EOF
fi
#==============================================================================

