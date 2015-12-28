#!/bin/bash
#. GetTopNames.sh 56
cp UNIQUE UNIQUE-NEANDERTHAL;
filename="UNIQUE-NEANDERTHAL"
while read -r line
do
    name=$line
    if [[ $line == "" ]] 
      then
      continue;
      fi;
    echo "PROCESSING: $name"
    perl downloadOrg.pl "$name"

    echo "DONE";
done < "$filename"

