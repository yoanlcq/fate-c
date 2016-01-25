#!/bin/bash

#After running this script, I advise to still check your files manually.
#The indentation is intentionally bad so it fits 80-columns screens.

function parsedir {
for file in $@; do
    if [[ $(basename $file) == README* ]]; then continue; fi
    if [[ $(basename $file) == *.md ]]; then continue; fi
    if [[ $(basename $file) == *.sh ]]; then continue; fi
    if [[ $(basename $file) == *.txt ]]; then continue; fi
    if [[ -d $file ]]; then parsedir $file/*; continue; fi
    cat notice.txt $file > /tmp/foobar;
    cp /tmp/foobar $file;
done
}
parsedir *;

