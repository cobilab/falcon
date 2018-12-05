#!/bin/bash
###############################################################################
# PERFORMANCE OF FALCON HANDLING INCREASING RANDOM PERMUTATION OF BLOCK SIZES
# REQUIREMENTS: linux OS with cmake, git and gnuplot
# (sudo apt-get install cmake git gnuplot)
###############################################################################
# PARAMETERS ==================================================================
RENEW=1;
INSTALL=1;
SIMULATE=1;
SHUFFLE=1;
FALCON=1;
GREEN=1;
MUMMER=1;
MUMMER20=1;
PLOT=1;
#==============================================================================
FQLINE=2000;
FQNREADS=2000;
PMIN=20;   #PERMUTATION MINIMUM
PMAX=250;  #PERMUTATION MAXIMUM
JUMP=5;    #JUMP BETWEEN LOOPS
#==============================================================================
DISTRIBUTION="0.3,0.2,0.2,0.3,0.000";
EXTRAMUT=" ";
#EXTRAMUT=" -ir 0.01 -dr 0.01 ";
FPARAM=" -m 20:1000:0:5/50 -c 20 -g 0.95 ";
GPARAM=" -rm 20:1000:1:5/50 -c 20 -g 0.95 ";
GREENPARAM=" -i -f 10 -k 15 ";
###############################################################################
if [[ "$RENEW" -eq "1" ]]; then
  rm -fr ptmp;
  mkdir ptmp;
fi
cd ptmp/
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
# SIMULATE ====================================================================
if [[ "$SIMULATE" -eq "1" ]]; then
./XS -v -ls $FQLINE -n $FQNREADS -f $DISTRIBUTION -s 0 SAMPLE.fq
# MUTATE ======================================================================
./goose-fastq2fasta < SAMPLE.fq > SAMPLE.fa
./goose-fasta2seq   < SAMPLE.fa > SAMPLE
./goose-seq2fasta -n "Permutation0" < SAMPLE > SAMPLE0.fa
#cat SAMPLE0.fa > DB.mfa;
for((x=$PMIN ; x<=$PMAX ; x+=$JUMP));
  do
  echo "Permutation rate: $x";
  ./goose-permuteseqbyblocks -bs $x -s 101 < SAMPLE > SAMPLE$x
  ./goose-seq2fasta -n "Permutation$x" < SAMPLE$x > SAMPLE$x.fa 
  cat SAMPLE$x.fa >> DB.mfa;
  done
fi
###############################################################################
# SHUFFLE READS ===============================================================
if [[ "$SHUFFLE" -eq "1" ]]; then
. ShufFASTQReads.sh SAMPLE.fq > SAMPLE-SHUF.fq
mv SAMPLE-SHUF.fq SAMPLE.fq
./goose-fastq2fasta < SAMPLE.fq > SAMPLE.fa
fi
###############################################################################
# RUN FALCON ==================================================================
if [[ "$FALCON" -eq "1" ]]; then
./FALCON -v -F $FPARAM -n 4 -t $PMAX -x TOP-SUBS SAMPLE.fq DB.mfa
#./FALCON -v -F $FPARAM -n 4 -t $PMAX -x TOP-SUBS SAMPLE0.fa DB.mfa
awk '{ print $4"\t"$3;}' TOP-SUBS | sed 's/\Permutation//g' | sort -n \
| awk '{ print $1"\t"$2;}' > TOP-SUBS-FILT;
fi
###############################################################################
# RUN GREEN ===================================================================
if [[ "$GREEN" -eq "1" ]]; then
rm -f TOP-GREEN;
cat SAMPLE.fa | grep -v ">" | tr -d -c "ACGTN" > SAMPLE.seq;
GSIZE=`./goose-info < SAMPLE.seq  | grep "Number of" | awk '{ print $4;}'`;
for((x=$PMIN ; x<=$PMAX ; x+=$JUMP));
  do
  printf "%u\t" "$x" >> TOP-GREEN;
  grep -v ">" SAMPLE$x.fa | tr -d -c "ACGTN" > SAMPLEX.seq;
  ./GReEnC -v $GREENPARAM SAMPLE.seq SAMPLEX.seq > TMPX;
  GVALUE=`grep "number of bytes" TMPX | awk '{ print $5;}'`;
  GRES=`echo "scale=6;100-((8*($GVALUE)/(2*$GSIZE))*100)" | bc -l`;
  echo $GRES >> TOP-GREEN;
  done
fi

###############################################################################
# RUN MUMMER ==================================================================
if [[ "$MUMMER" -eq "1" ]]; then
rm -f TOP-MUMMER;
for((x=$PMIN ; x<=$PMAX ; x+=$JUMP));
  do
  printf "%u\t" "$x" >> TOP-MUMMER;
  ./nucmer -p mummer-tmp SAMPLE.fa SAMPLE$x.fa
  ./delta-filter -1 mummer-tmp.delta > mummer-tmp.delta2
  ./show-coords -clr mummer-tmp.delta2 > mummer-tmp.delta3
  echo "Running Global similarity for MUMmer ...";
  . GlobalMUMmer.sh mummer-tmp.delta3 >> TOP-MUMMER;
  done
fi
###############################################################################
# RUN MUMMER 20 ===============================================================
if [[ "$MUMMER20" -eq "1" ]]; then
rm -f TOP-MUMMER20;
for((x=$PMIN ; x<=$PMAX ; x+=$JUMP));
  do
  printf "%u\t" "$x" >> TOP-MUMMER20;
  #./nucmer --maxmatch -c 20 -p mummer-tmp SAMPLE.fa SAMPLE$x.fa
  ./nucmer -c 20 -p mummer-tmp SAMPLE.fa SAMPLE$x.fa
  ./delta-filter -1 mummer-tmp.delta > mummer-tmp.delta2
  ./show-coords -clr mummer-tmp.delta2 > mummer-tmp.delta3
  echo "Running Global similarity for MUMmer -c 20 ...";
  . GlobalMUMmer.sh mummer-tmp.delta3 >> TOP-MUMMER20;
  done
fi
###############################################################################
# PLOT ========================================================================
if [[ "$PLOT" -eq "1" ]]; then
gnuplot << EOF
set terminal pdfcairo enhanced color font 'Verdana,14'
set output "per.pdf"
set auto
set size ratio 0.75
set key outside right center
set yrange [0:100] 
h(x)=(-x*log(x)-(1-x)*log(1-x))/log(2)
unset grid
set ylabel "Relative similarity (%)"
set xlabel "Permutation block size"
set style line 1 lt 2 lc rgb "#CA000D" lw 5
set style line 2 lt 2 lc rgb "#018D0F" lw 5
set style line 3 lt 2 lc rgb "#000DC0" lw 5
set style line 4 lt 2 lc rgb "#FF811F" lw 5
set style line 5 lt 2 lc rgb "black" lw 5
plot [$PMAX:$PMIN] "TOP-MUMMER" u 1:2 w lines ls 3 title "MUMmer", \
 "TOP-MUMMER20" u 1:2 w lines ls 4 title "MUMmer -c 20", \
 "TOP-GREEN" u 1:2 w lines ls 2 title "GREEN", \
 "TOP-SUBS-FILT" u 1:2 w lines ls 1 title "FALCON", \
 (1-(1./x)*log(1.0*$FQLINE*$FQNREADS)/log(2.)/2.)*100 w lines ls 5 title "Theoretical"
EOF
#(1-(1./x)*log(1.0*$FQLINE*$FQNREADS)/log(2.)/2.)*100 w lines ls 5 title "Theoretical Fix", \
#(1-((1./x)*(2*log(1.0*$FQLINE*$FQNREADS)/log(2.)-1)/2.))*100  w lines ls 5 title "Theoretical var"
fi
#==============================================================================
