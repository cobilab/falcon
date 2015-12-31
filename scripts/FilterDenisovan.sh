#!/bin/bash
. RunHumanDenisovan.sh
. GetTopNames.sh 13
cat UNIQUE | tr -d "." > bnames.nm;
perl DownloadRef.pl bnames.nm;
