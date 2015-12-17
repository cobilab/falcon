#!/bin/bash
#
# ===========----------------------------------
# | WARNING | THIS REQUIRES 1 TB OF FREE DISK |
# ===========----------------------------------
#
# DEPENDECY: none;
#
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EVANM="http://cdna.eva.mpg.de";
EVAPT="$EVANM/denisova/raw_reads";
WGETO=" --trust-server-names -q ";
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
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
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
