#!/bin/bash
#=============================================================================
INITALS="CHM";
MAX=22;
WGETOP=" --trust-server-names -q ";
ONWAY="ftp://ftp.ncbi.nlm.nih.gov/genomes/Homo_sapiens/Assembled_chromosomes/seq/hs_alt_CHM1_1.1_chr"
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
  ZPATH="$ONWAY$x.fa.gz";
  downloadEach "$WGETOP" "$ZPATH" "$x" "$INITALS";
  zcat $INITALS-X$x > $INITALS$x;
  echo "$INITALS C$x filtered!";
  done

CHR=23;
FIELD="X";
ZPATH="$ONWAY$FIELD.fa.gz";
downloadEach "$WGETOP" "$ZPATH" "$CHR" "$INITALS";
zcat $INITALS-X$CHR > $INITALS$CHR;
echo "$INITALS CX filtered";

rm *CHM-* -f
echo "Done!"
#=============================================================================
