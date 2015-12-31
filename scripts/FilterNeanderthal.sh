#!/bin/bash
. RunHumanNeanderthal.sh
. GetTopNamesNeanderthal.sh
cat UNIQUE-NEAN | tr -d "." > bnames-nean.nm;
perl DownloadRef.pl bnames-nean.nm;
