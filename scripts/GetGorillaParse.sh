#!/bin/bash
#=============================================================================
INITALS="GG";
MAX=22;
WGETOP=" --trust-server-names -q ";
ONWAY="ftp://ftp.ncbi.nlm.nih.gov/genomes/Gorilla_gorilla/Assembled_chromosomes/seq/ggo_ref_gorGor3.1_chr"
#-----------------------------------------------------------------------------
function downloadEach
  {
  PATTERN="unexpected";
  for((;;)); 
    do
    sleep 2;
    xtmp="`wget $1 $2 -O $4-X$3`"
    if [[ "$xtmp" == *"$PATTERN"* ]];  
      then
      echo "Unexpected EOF found, re-downloading C$3 ...";
      continue;
    else
      echo "wget stderr:$xtmp";
      echo "Downloaded $4 C$3 with success!";
      break;
    fi
    done
  }

echo "Downloading and filtering $INITALS sequences ..."
for((x=1 ; x <= $MAX ; ++x)); 
  do
  if [ $x -ne "2" ]; 
    then
    ZPATH="$ONWAY$x.fa.gz";
    downloadEach "$WGETOP" "$ZPATH" "$x" "$INITALS";
    zcat $INITALS-X$x > $INITALS$x;
    echo "$INITALS C$x filtered!";
  fi
  done

ZPATH=$ONWAY"2A.fa.gz";
downloadEach "$WGETOP" "$ZPATH" "2A" "$INITALS";
zcat $INITALS-X2A > $INITALS"2A";
echo "$INITALS C2A filtered";

ZPATH=$ONWAY"2B.fa.gz";
downloadEach "$WGETOP" "$ZPATH" "2B" "$INITALS";
zcat $INITALS-X2B > $INITALS"2B";
echo "$INITALS C2B filtered";

CHR=23;
FIELD="X";
ZPATH="$ONWAY$FIELD.fa.gz";
downloadEach "$WGETOP" "$ZPATH" "$CHR" "$INITALS";
zcat $INITALS-X$CHR > $INITALS$CHR;
echo "$INITALS CX filtered";

CHR=24;
downloadEach "$WGETOP" "ftp://ftp.ncbi.nlm.nih.gov/genomes/Gorilla_gorilla/Assembled_chromosomes/seq/ggo_ref_gorGor3.1_unplaced.fa.gz" "$CHR" "$INITALS";
zcat $INITALS-X$CHR > $INITALS$CHR;
echo "$INITALS UNPLACED filtered";

CHR=25;
downloadEach "$WGETOP" "ftp://ftp.ncbi.nlm.nih.gov/genomes/Gorilla_gorilla/Assembled_chromosomes/seq/ggo_ref_gorGor3.1_chrMT.fa.gz" "$CHR" "$INITALS";
zcat $INITALS-X$CHR > $INITALS$CHR;
echo "$INITALS MITOCONDRIA filtered";

rm -f *GG-* $INITALS-X2A $INITALS-X2B;
cat GG* > GG
echo "Done!"
#=============================================================================

