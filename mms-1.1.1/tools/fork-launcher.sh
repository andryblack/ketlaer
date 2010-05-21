#!/bin/bash

# this is a generic wrapper to use with mms (My Media System)
# when you need to launch an external player that forks
# See: http://mms.kicks-ass.org/

LOGFILE=""


help ()
{
   echo "
   This is a generic wrapper for mms (My Media System)
   you can use to launch external players that fork()
      
   Usage: ${0/*\/} [OPTION1] <PROGRAM> <CMD LINE>

   OPTION1 can be one of the following:
   -h      Show this help

   This script executes <CMD LINE> and then wait till
   <PROGRAM> exits.
            
"
}

disclaimer ()
{
   echo "
   My Media System is an application that manages, displays
   and plays media content such as videos, music, pictures,
   and more. MMS runs perfectly on anything from a Set-Top-Box
   connected to your TV-Set, to your specially tailored
   multimedia PC and HD display.

   See: http://mms.sunsite.dk/

   "
}

# we need *at least* 2 parameters, otherwise we
# show an error message
if [ $# -lt 2 ] ; then

   echo "Error!!!"
   help
   exit  1

fi

case "$1" in
   -h)
      disclaimer
         help
      exit 0
   ;;
   
   -*)   
      echo "Error: unknown option $1"
      help
      exit 1
   ;;
      
esac

CMD="$2"
CMDNAME="${1}"
shift
shift

sh -c  "$CMD $* "  

while ((1)) ; do
   pidof $CMDNAME > /dev/null 2>&1 || exit    
   sleep 1
done

exit
