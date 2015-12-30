#!/bin/bash
# 
# AN EXAMPLE SHOWING THE DETECTION OF VIRUS SUBSEQUENCES 
# IN THE SYMBIONT METAGENOME, WGS SEQUENCING PROJECT.
#
# INSTALL FALCON ==============================================================
git clone https://github.com/pratas/falcon.git
cd falcon/src/
cp Makefile.linux Makefile
make
cp FALCON ../../
cd ../../
cp falcon/scripts/DownloadViruses.pl .
rm -fr falcon/
# GET SYMBIONT WGS ============================================================
WGETO=" --trust-server-names -q ";
BEPAT=" http://www.ncbi.nlm.nih.gov/Traces/wgs"
echo "Downloading and uncompressing SYMBIONT WGS ...";
wget $WGETO $BEPAT/?download=AASZ01.1.fsa_nt.gz -O AASZ01.1.fsa_nt.gz
gunzip AASZ01.1.fsa_nt.gz
echo "Done!";
# GET VIRUSES =================================================================
perl DownloadViruses.pl
# RUN FALCON ==================================================================
./FALCON -v -F -l 32 -c 20 -g 0.98 -n 4 AASZ01.1.fsa_nt viruses.fa
# ============================================================================= 
