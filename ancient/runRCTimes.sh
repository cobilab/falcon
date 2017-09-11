#!/bin/bash
#
GET_FALCON=1;
GET_SAMTOOLS=1;
GET_GOOSE=1;
GET_BOWTIE=1;
GET_BWA=1;
GET_NEANDERTHAL=1;
GET_ECOLI=1
RUN_BOWTIE=1;
RUN_BOWTIE_ANCIENT=1;
RUN_BWA=1;
RUN_BWA_ANCIENT=1;
RUN_FALCON=1;
#
#==============================================================================
# GET BOWTIE
#
if [[ "$GET_BOWTIE" -eq "1" ]]; then
  #sudo apt-get install libtbb-dev
  rm -fr bowtie/
  git clone https://github.com/BenLangmead/bowtie.git
  cd bowtie/
  make
  cd ..
fi
#==============================================================================
# GET BWA
#
if [[ "$GET_BWA" -eq "1" ]]; then
  rm -fr bwa/
  git clone https://github.com/lh3/bwa.git
  cd bwa/
  make
  cd ..
fi
#==============================================================================
# GET FALCON
#
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
# GET SAMTOOLS 1.3.1
if [[ "$GET_SAMTOOLS" -eq "1" ]]; then
  wget https://github.com/samtools/samtools/releases/download/1.3.1/samtools-1.3.1.tar.bz2
  tar -xvf samtools-1.3.1.tar.bz2
  cd samtools-1.3.1/
  ./configure --without-curses
  make
  cp samtools ../
  cd ..
  rm -fr samtools-1.3.1.*
fi
#==============================================================================
# GET NEANDERTHAL
if [[ "$GET_NEANDERTHAL" -eq "1" ]]; then
  # ===========---------------------------------------------
  # | WARNING | THIS REQUIRES AT LEAST 300 GB OF FREE DISK |
  # ===========---------------------------------------------
  #
  # DEPENDECY: SAMTOOLS;
  #
  EVANM="http://cdna.eva.mpg.de";
  EVAPT="$EVANM/neandertal/altai/AltaiNeandertal/bam";
  EVAPK="$EVANM/neandertal/altai/AltaiNeandertal/bam/unmapped_qualfail/";
  WGETO=" --trust-server-names -q ";
  echo "Downloading sequences ... (This may take a while!)";

  #for((x=1 ; x<=22 ; ++x));  # GET NEANTHERTAL GENOME IN BAM FORMAT
  #  do
  #  wget $WGETO $EVAPT/AltaiNea.hg19_1000g.$x.dq.bam -O HN-C$x.bam;
  #  done
  #wget $WGETO $EVAPT/AltaiNea.hg19_1000g.Y.dq.bam -O HN-C24.bam;

  # ONLY UNMAPPED DATA:
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
  rm -f NEAN-UM.fq;
  for((x=25 ; x<=56 ; ++x)); # ONLY UNMAPPED DATA
    do
    ./samtools bam2fq HN-C$x.bam \
    | ./goose-FastqMinimumLocalQualityScoreForward -k 5 -w 15 -m 33 \
    | ./goose-FastqMinimumReadSize 35 \
    | ./goose-fastq2mfasta >> NEAN-UM.fq
    rm -f HN-C$x;
    done
fi
#==============================================================================
# DOWNLOAD ECOLI
#
if [[ "$GET_ECOLI" -eq "1" ]]; then
  rm -f GCF_000005845.2_ASM584v2_genomic.fna.* ECOLI.fna;
  wget ftp://ftp.ncbi.nlm.nih.gov/genomes/refseq/bacteria/Escherichia_coli/reference/GCF_000005845.2_ASM584v2/GCF_000005845.2_ASM584v2_genomic.fna.gz
  gunzip GCF_000005845.2_ASM584v2_genomic.fna.gz
  mv GCF_000005845.2_ASM584v2_genomic.fna ECOLI.fa
fi
#==============================================================================
# RUN BOWTIE
#
if [[ "$RUN_BOWTIE" -eq "1" ]]; then
  (time ./bowtie/bowtie-build ECOLI.fa index-ECOLI ) &> REPORT_BOWTIE_1;
  (time ./bowtie/bowtie -a --sam index-ECOLI NEAN-UM.fq > OUT_ALIGNED_BOWTIE.sam ) &> REPORT_BOWTIE_2;
fi
#==============================================================================
# RUN BOWTIE V3
#
if [[ "$RUN_BOWTIE_ANCIENT" -eq "1" ]]; then
  (time ./bowtie/bowtie-build ECOLI.fa index-ECOLI ) &> REPORT_BOWTIE_ANCIENT_1;
  (time ./bowtie/bowtie -a -v 3 --sam index-ECOLI NEAN-UM.fq > OUT_ALIGNED_BOWTIE_ANCIENT.sam ) &> REPORT_BOWTIE_ANCIENT_2;
fi
#==============================================================================
# RUN BWA
#
if [[ "$RUN_BWA" -eq "1" ]]; then
  (time ./bwa/bwa index ECOLI.fa ) &> REPORT_BWA_1;
  (time ./bwa/bwa mem ECOLI.fa NEAN-UM.fq | gzip -3 > OUT_ALIGNED_BWA.sam.gz ) &> REPORT_BWA_2;
fi
#==============================================================================
# RUN BWA ANCIENT
#
if [[ "$RUN_BWA_ANCIENT" -eq "1" ]]; then
  (time ./bwa/bwa index ECOLI.fa ) &> REPORT_BWA_ANCIENT_1;
  (time ./bwa/bwa mem -L 16500 -N 0.01 -O 2 ECOLI.fa NEAN-UM.fq | gzip -3 > OUT_ALIGNED_BWA_ANCIENT.sam.gz ) &> REPORT_BWA_ANCIENT_2;
fi
#==============================================================================
# RUN FALCON
#
if [[ "$RUN_FALCON" -eq "1" ]]; then
  (time ./FALCON -v -n 8 -t 500 -F -m 13:20:0/0 -m 20:200:1:5/20 -c 200 NEAN-UM.fq ECOLI.fa ) &> REPORT_FALCON;
fi
#==============================================================================

