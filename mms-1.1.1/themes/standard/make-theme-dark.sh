#!/bin/sh
#Andreas Saur - 2007-11-18
rm -rf general/bg-4-3.png
rm -rf general/bg-10-9.png
rm -rf general/bg-16-10.png
rm -rf startmenu/bg-4-3.png
rm -rf startmenu/bg-16-10.png
rm -rf startmenu/bg-16-9.png
cd startmenu
ln -f -s bg-4-3-dark.png bg-4-3.png
ln -f -s bg-16-9-dark.png bg-16-9.png
ln -f -s bg-16-10-dark.png bg-16-10.png
cd ../general
ln -f -s bg-4-3-dark.png bg-4-3.png
ln -f -s bg-16-9-dark.png bg-16-9.png
ln -f -s bg-16-10-dark.png bg-16-10.png
cd ..
cp theme-dark.conf theme.conf
echo Theme is darker now. You need to restart MMS in order to see the changes.
