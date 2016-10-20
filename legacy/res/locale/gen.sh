#!/bin/bash

year=2016
author="Yoan Lecoq"
email=yoanlecoq.io@gmail.com
localedir=res/locale
package=fate
package_version=1.0
lang=fr
locale=fr_FR

mkdir -p $localedir/$lang/LC_MESSAGES
# xgettext --package-name ${package} --package-version 1.0 -d ${package} -o $localedir/${package}.pot -k_ -s hellogt.c
xgettext -d ${package} -o $localedir/${package}.pot \
    -c -k_ -kP_ -w 80 -s \
    --copyright-holder "$year $author" \
    --package-name ${package} \
    --package-version $package_version \
    --msgid-bugs-address $email \
    hellogt.c
msginit -i $localedir/${package}.pot \
        -o $localedir/$lang/${package}.po \
        -l $locale \
        -w 80 \
        --no-translator 
vim $localedir/$lang/${package}.po
msgfmt -c -v -o $localedir/$lang/LC_MESSAGES/${package}.mo $localedir/$lang/${package}.po
