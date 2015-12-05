#!/bin/bash
#
# ===========---------------------------------------------
# | WARNING | THIS REQUIRES AT LEAST 300 GB OF FREE DISK |
# ===========---------------------------------------------
#
# DEPENDECY: SAMTOOLS;
#
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EVANM="http://cdna.eva.mpg.de";
EVAPT="$EVANM/neandertal/altai/AltaiNeandertal/bam";
EVAPK="$EVANM/neandertal/altai/AltaiNeandertal/bam/unmapped_qualfail/";
WGETO=" --trust-server-names -q ";

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
echo "Downloading sequences ... (This may take a while!)";
for((x=1 ; x<=22 ; ++x));  # GET NEANTHERTAL GENOME IN BAM FORMAT
  do
  # AUTOSSOMES DATA - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  wget $WGETO $EVAPT/AltaiNea.hg19_1000g.$x.dq.bam -O HN-C$x.bam;
  done
# SEXUAL CHROMOSOMES DATA - - - - - - - - - - - - - - - - - - - - - - - - - - - 
wget $WGETO $EVAPT/AltaiNea.hg19_1000g.X.dq.bam -O HN-C23.bam;
wget $WGETO $EVAPT/AltaiNea.hg19_1000g.Y.dq.bam -O HN-C24.bam;
# UNMAPPED DATA - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
wget $WGETO $EVAPK/NIOBE_0139_A_D0B5GACXX_7_unmapped.bam -O HN-C25.bam;
wget $WGETO $EVAPK/NIOBE_0139_A_D0B5GACXX_8_unmapped.bam -O HN-C26.bam;
wget $WGETO $EVAPK/SN928_0068_BB022WACXX_1_unmapped.bam -O HN-C27.bam;
wget $WGETO $EVAPK/SN928_0068_BB022WACXX_2_unmapped.bam -O HN-C28.bam;
wget $WGETO $EVAPK/SN928_0068_BB022WACXX_3_unmapped.bam -O HN-C29.bam;
wget $WGETO $EVAPK/SN928_0068_BB022WACXX_4_unmapped.bam -O HN-C30.bam;
wget $WGETO $EVAPK/SN928_0068_BB022WACXX_5_unmapped.bam -O HN-C31.bam;
wget $WGETO $EVAPK/SN928_0068_BB022WACXX_6_unmapped.bam -O HN-C32.bam;
wget $WGETO $EVAPK/SN928_0068_BB022WACXX_7_unmapped.bam -O HN-C33.bam;
wget $WGETO $EVAPK/SN928_0068_BB022WACXX_8_unmapped.bam -O HN-C34.bam;
wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_1_unmapped.bam -O HN-C35.bam;
wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_2_unmapped.bam -O HN-C36.bam;
wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_3_unmapped.bam -O HN-C37.bam;
wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_4_unmapped.bam -O HN-C38.bam;
wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_5_unmapped.bam -O HN-C39.bam;
wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_6_unmapped.bam -O HN-C40.bam;
wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_7_unmapped.bam -O HN-C41.bam;
wget $WGETO $EVAPK/SN928_0073_BD0J78ACXX_8_unmapped.bam -O HN-C42.bam;
wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_1_unmapped.bam -O HN-C43.bam;
wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_2_unmapped.bam -O HN-C44.bam;
wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_3_unmapped.bam -O HN-C45.bam;
wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_5_unmapped.bam -O HN-C46.bam;
wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_6_unmapped.bam -O HN-C47.bam;
wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_7_unmapped.bam -O HN-C48.bam;
wget $WGETO $EVAPK/SN7001204_0130_AC0M6HACXX_PEdi_SS_L9302_L9303_1_8_unmapped.bam -O HN-C49.bam;
wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_1_unmapped.bam -O HN-C50.bam;
wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_2_unmapped.bam -O HN-C51.bam;
wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_3_unmapped.bam -O HN-C52.bam;
wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_5_unmapped.bam -O HN-C53.bam;
wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_6_unmapped.bam -O HN-C54.bam;
wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_7_unmapped.bam -O HN-C55.bam;
wget $WGETO $EVAPK/SN7001204_0131_BC0M3YACXX_PEdi_SS_L9302_L9303_2_8_unmapped.bam -O HN-C56.bam;
# FROM SAM 2 MFASTA - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
for((xi=1 ; xi<=56 ; ++xi));
  do
  samtools view HN-C$xi.bam | awk '{OFS="\t"; print ">"$1"\n"$10}' > HN-C$xi ;
  done
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

