#!/bin/bash
. RunHumanNeanderthal.sh
. GetTopNames.sh 56
cat UNIQUE | tr -d "." > bnames.nm;
perl DownloadRef.pl bnames.nm;

#filename="U"
#while read -r line
#do
#    name=$line
#    if [[ $line == "" ]] 
#      then
#      continue;
#      fi;
#    echo "PROCESSING: $name"
#    perl downloadOrg.pl "$name"
#
#    echo "DONE";
#done < "$filename"

