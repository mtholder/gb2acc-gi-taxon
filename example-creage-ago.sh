#!/bin/sh
# looks for *.seq.xz files and runs gb2acc-gi-taxon on them.
set -x
ff='failed-ago-err-list.txt'
rm -f "${ff}"
for f in *.seq.xz
do
    fn=$(echo $f | cut -d. -f1-2)
    echo $fn
    if ! test -f "$fn"
    then
        xz --decompress --stdout "$f" > "$fn" || exit
    fi
    if ! gb2acc-gi-taxon "$fn" > "${fn}-ago.tsv" 2>"${fn}-ago-err.txt"
    then
        echo "$fn" >> "$ff"
    fi
done
if test -f "${ff}"
then
    echo "The following files generated errors rather than just warnings:"
    cat "$ff"
else
    echo "SUCCESS! no errors (there may be warnings in the -err.txt files)"
fi
