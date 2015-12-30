#!/bin/bash
git clone https://github.com/pratas/falcon.git
cd falcon/src/
cmake .
make
cd ../../
cp falcon/src/FALCON .
cp falcon/scripts/GetHumanGRCParse.sh .
cp falcon/scripts/DownloadBacteria.pl .
rm -fr falcon/
perl DownloadBacteria.pl
. GetHumanGRCParse.sh
rm -f GRC;
for((x=1; x<=23 ; ++x)); do cat GRC$x >> GRC; rm -f GRC$x; done
(time ./FALCON -v -F -l 38 -c 90 -n 1 -t 200000 -x TOP-GRC GRC Bacteria.fa ) &> REPORT-GRC
