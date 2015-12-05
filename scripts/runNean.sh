#!/bin/bash
MYMAIL="pratas@ua.pt";
#
# INSTALL =====================================================================
#
. install.sh
#
# DOWNLOAD ====================================================================
perl downloadViruses.pl
echo "Downloaded virus base to sapiens ~/COMPLETE-NEANDERTHAL" | mail -s "Downloaded virus base..." $MYMAIL
perl downloadBacteria.pl
echo "Downloaded bacteria base to sapiens ~/COMPLETE-NEANDERTHAL" | mail -s "Downloaded bacteria base..." $MYMAIL
. GetHumanNeand.sh
echo "Downloaded neanderthal base to sapiens ~/COMPLETE-NEANDERTHAL" | mail -s "Downloaded neanderthal base..." $MYMAIL
# =============================================================================

for((xi=1 ; xi<=56 ; ++xi));
  do
  (time ./FALCON -v -l 23 -p 1 -t 500 -n 10 -c 50 -x TOP$xi HN-C$xi viruses.fa) &> REPORT$xi;
  cat TOP$xi | mail -s "Neanderthal C$xi done! (COMPLETE-NEANDERTHAL)" $MYMAIL
  done
