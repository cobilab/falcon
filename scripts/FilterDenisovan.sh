#!/bin/bash
. RunHumanDenisovan.sh
. GetTopNamesDenisovan.sh
cat UNIQUE-DENI | tr -d "." > bnames-deni.nm;
perl DownloadRef.pl bnames-deni.nm;
