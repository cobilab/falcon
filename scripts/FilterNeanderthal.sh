#!/bin/bash
. RunHumanNeanderthal.sh
. GetTopNames.sh 56
cat UNIQUE | tr -d "." > bnames.nm;
perl DownloadRef.pl bnames.nm;
