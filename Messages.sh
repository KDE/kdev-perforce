#!/bin/sh
$EXTRACTRC *.rc >> rc.cpp
$XGETTEXT *.cpp -o $podir/kdevperforce.pot
rm -f rc.cpp
