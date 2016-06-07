#!/bin/bash
# 
# AN EXAMPLE SHOWING THE DETECTION OF A VIRUS IN THE BALD EAGLE GENOME
# FOR THE PURPOSE IT IS ONLY REQUIRED TO RUN THIS SCRIPT
#
# INSTALL FALCON ==============================================================
rm -fr falcon/
git clone https://github.com/pratas/falcon.git
cd falcon/src/
cp Makefile.linux Makefile
make
cp FALCON ../../
cp FALCON-FILTER ../../
cp FALCON-EYE ../../
cd ../../
cp falcon/scripts/DownloadViruses.pl .
rm -fr falcon/
# GET BALD EAGLE ==============================================================
WGETO=" --trust-server-names -q ";
BEPAT=" ftp://climb.genomics.cn/pub/10.5524/101001_102000/101040"
echo "Downloading and decompressing Bald Eagle ...";
wget $WGETO $BEPAT/Haliaeetus_leucocephalus.fa.gz -O BE.fa.gz
gunzip BE.fa.gz
echo "Done!";
# GET VIRUSES =================================================================
perl DownloadViruses.pl
# RUN FALCON ==================================================================
./FALCON -v -n 4 -t 200 -F -Z -m 20:100:1:5/10 -c 45 -y com.x BE.fa viruses.fa
./FALCON-FILTER -v -F -t 0.5 -o positions.pos com.x
./FALCON-EYE -v -F -o draw.map positions.pos
# ============================================================================= 
