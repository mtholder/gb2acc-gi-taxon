#!/bin/sh
# looks for *.seq.xz files and runs gb2acc-gi-taxon on them.
set -x
ff='failed-ago-err-list.txt'
rm -f "${ff}"
rm -f "COMBINED-ago.tsv"
rm -f "COMBINED-ago-err.txt"
for f in *.seq.xz
do
    fn=$(echo $f | cut -d. -f1-2)
    echo $fn
    if ! test -f "${fn}-ago.tsv"
    then
        uncompressed_file=0
        if ! test -f "$fn"
        then
            xz --decompress --stdout "$f" > "$fn" || exit
            uncompressed_file=1
        fi
        if ! gb2acc-gi-taxon "$fn" > "${fn}-ago.tsv" 2>"${fn}-ago-err.txt"
        then
            echo "$fn" >> "$ff"
        fi
        if test $uncompressed_file -eq 1
        then
            rm "$fn"
        fi
    fi
    cat "${fn}-ago.tsv" >> "COMBINED-ago.tsv"
    cat "${fn}-ago-err.txt" >> "COMBINED-ago-err.txt"
done
if test -f "${ff}"
then
    echo "The following files generated errors rather than just warnings:"
    cat "$ff"
else
    echo "SUCCESS! no errors (there may be warnings in the -err.txt files)"
fi
