#!/bin/sh
combotsv=COMBINED-ago.tsv
comboerr=COMBINED-ago-err.txt
finaltsv="$PWD/$combotsv"
finalerr="$PWD/$comboerr"
rsd=release-subdirs.txt
set -x

rm -f "$finaltsv" "$finalerr"
for d in $(cat "$rsd")
do
    cd $d
    create-ago-xz.sh || exit
    cd -
    cat "${d}/${combotsv}" >> "${finaltsv}"
    cat "${d}/${comboerr}" >> "${finalerr}"
done

# daily snap shots
dailydir=$(cat daily-subdir.txt)
cd "${dailydir}"
rsync --partial --progress -av ftp.ncbi.nlm.nih.gov::genbank/daily-nc/*.flat.gz . || exit
create-ago-gz.sh || exit
cd -
cat "${dailydir}/${combotsv}" >> "${finaltsv}"
cat "${dailydir}/${comboerr}" >> "${finalerr}"

