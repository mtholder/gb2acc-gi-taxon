#!/bin/sh

for div in 
do
    mkdir gb$div
    rsync --partial --progress -av ftp.ncbi.nlm.nih.gov::genbank/gb${div}*.gz gb${div}
    cd gb$div
    for f in gb*seq.gz 
    do
        x=$(echo $f | cut -d. -f1-2)
        gunzip $x.gz 
        xz -9 $x 
    done
    cd -
    $PWD/gb$div >> release-subdirs.txt
done

