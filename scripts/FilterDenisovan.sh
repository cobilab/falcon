#!/bin/bash
. RunHumanDenisovan.sh
. GetTopNamesDenisovan.sh
tr -d "." < UNIQUE-DENI > bnames-deni.nm;
perl DownloadRef.pl bnames-deni.nm;
