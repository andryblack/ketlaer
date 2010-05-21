#!/bin/bash

# example script used for updating of the EPG tv.xml data file
#
# German users might use 'tv_grab_de_prisma' from here:
# http://sfr.mythwiki.de/index.html
#
# After the installation of xmltv your grabber has to be configured:
#
# tv_grab_(your choice) --configure [--gui tk]
#
# To make MMS automaticly use this script to update the EPG listings 
# once each day, define where you have placed the script using:
#
# epg_update_script = /usr/lib/mms/gen_tvlisting.sh 

DATADIR=/var/lib/mms
TMPDIR=/var/cache/mms/ 

echo "Updating TV.xml, please be patient"
tv_grab_dk --config-file ~/.xmltv/tv_grab_dk.conf --output $TMPDIR/TV.xml.tmp
iconv --from-code=ISO-8859-1 --to-code=UTF-8 < $TMPDIR/TV.xml.tmp > $TMPDIR/TV.utf8.xml
mv $TMPDIR/TV.utf8.xml $DATADIR/TV.xml
rm $TMPDIR/TV.xml.tmp
