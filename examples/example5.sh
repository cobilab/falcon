#!/bin/bash
# 
# AN EXAMPLE SHOWING INFERENCE OF PATHOGENS IN THE HUMAN GUT MICROBIOME
# FOR THE PURPOSE IT IS ONLY REQUIRED TO RUN THIS SCRIPT
#
# INSTALL FALCON ==============================================================
git clone https://github.com/pratas/falcon.git
cd falcon/src/
cp Makefile.linux Makefile
make
cp FALCON ../../
cd ../../
cp falcon/scripts/DownloadBacteria.pl .
rm -fr falcon/
# GET HUMAN GUT MICROBIOME ====================================================
WGETO=" --trust-server-names ";
XBEPAT="trace.ncbi.nlm.nih.gov/Traces/sra/";
BEPAT=$XBEPAT"sra.cgi?cmd=dload&format=fasta&run_list=";
echo "Downloading SRR2822456 ...";
wget $WGETO $BEPAT"SRR2822456" -O SRR2822456.fq.gz
gunzip SRR2822456.fq.gz
echo "Downloading SRX1373785 ...";
wget $WGETO $BEPAT"SRX1373785" -O SRX1373785.fq.gz
gunzip SRX1373785.fq.gz
echo "Downloading SRX1373786 ...";
wget $WGETO $BEPAT"SRX1373786" -O SRX1373786.fq.gz
gunzip SRX1373786.fq.gz
echo "Downloading SRX1373787 ...";
wget $WGETO $BEPAT"SRX1373787" -O SRX1373787.fq.gz
gunzip SRX1373787.fq.gz
echo "Done!";
# GET BACTERIA ================================================================
perl DownloadBacteria.pl
# RUN FALCON ==================================================================
./FALCON -v -F -l 15 -n 4 -t 1000 -x top-SRR2822456 SRR2822456.fq Bacteria.fa
./FALCON -v -F -l 15 -n 4 -t 1000 -x top-SRX1373785 SRX1373785.fq Bacteria.fa
./FALCON -v -F -l 15 -n 4 -t 1000 -x top-SRX1373786 SRX1373786.fq Bacteria.fa
./FALCON -v -F -l 15 -n 4 -t 1000 -x top-SRX1373787 SRX1373787.fq Bacteria.fa
# ============================================================================= 

