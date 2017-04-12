#!/bin/bash
#
READS_FILE="/home/pratas/DATABASE/NEANDERTHAL/FASTQ/NEAN.fq";
GET_FALCON=1;
GET_BOWTIE=1;
GET_ECOLI=1
RUN_FALCON=1;
RUN_BOWTIE=1;
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
  (time ./bowtie/bowtie -a -v 3 --sam index-ECOLI $READS_FILE > OUT_ALIGNED.sam ) &> REPORT_BOWTIE_2;
fi
#
#==============================================================================
# RUN FALCON
#
if [[ "$RUN_FALCON" -eq "1" ]]; then
  (time ./FALCON -v -n 8 -t 1000 -F -m 20:100:1:5/10 -c 200 $READS_FILE ECOLI.fa ) &> REPORT_FALCON;
fi
#==============================================================================

