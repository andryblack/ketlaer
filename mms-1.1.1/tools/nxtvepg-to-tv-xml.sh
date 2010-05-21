#!/bin/sh
#
# NAME /etc/mms/gen_epglisting.sh
#
# PREREQS analog TV adapter (cable or terrestial)
# configured and working vbi interfave
# nxtvepg set up and running
# - please test using it with GUI
# - select received channels
# - configure air times for part time channels
# - do additional setup - if you like
# correct the path variables MMSDIR and EPGDIR
# and manual definition of nxtvepg alias if needed
#
# FUNCTION extract analog EPG data provided on some TV channels
# select channel with biggest amount and recent update
# and proveide data in XML format
#
# AUTHOR Andreas Neuper (C)2006 under GPL
#
#______________________________________________________________
#
MMSDIR=/etc/mms
EPGDIR=/var/tmp/nxtvdb
#______________________________________________________________
#
# you might want to define an alias here:
# alias nxtvepg="/usr/local/bin/nxtvepg -card 0"
#
if [ -n "$(which nxtvepg)" ]
then
# maybe the path is not set correctly
if [ -x /usr/local/bin/nxtvepg ]
then
alias nxtvepg=/usr/local/bin/nxtvepg
else
# invoke another script that does it better
if [ -x /etc/mms/gen_tvlisting.sh ]
then
# prohibit eternal loop
if [ ! "$(basename $0)" = "gen_tvlisting.sh" ]
then
/etc/mms/gen_tvlisting.sh
fi
fi
# and die without complaints
exit 0
fi
fi
#______________________________________________________________
#
# maybe nxtvepg is installed but unconfigured
# do not bother anybody ...
test $(ls /var/tmp/nxtvdb/ |wc -w) -lt 3 && exit 0
#______________________________________________________________
#______________________________________________________________
#
# avoid DB updates during XML export
#
nxtvepg -daemonstop || echo "No NxTVepg daemon found"
#______________________________________________________________
#
for db in $EPGDIR/nxtvdb-*
do
provider=$(echo $db|sed 's:.*-::')
nxtvepg -provider $provider -dump xml >$MMSDIR/TV-$provider.xml
done
#______________________________________________________________
#
# do now replace the TV.xml file by
# the largest retrieved DB [ls -s | sort -n | tail -1]
# that is still active [-atime -1]
#
rm -f $MMSDIR/TV.xml || echo "No TV.xml present yet"
largeDB=$(find $EPGDIR -name 'nxtvdb-*' -mtime -1 -exec ls -s {} \; | sort -n | tail -1 | sed -e 's:.*-::')
ln -s $MMSDIR/TV-${largeDB}.xml $MMSDIR/TV.xml
#______________________________________________________________
#
# get EPG gathering/acquisition working again
#
nxtvepg -daemon
#
exit 0
#______________________________________________________________
#
