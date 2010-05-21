#! /bin/bash
#
# vboxtowav.sh
# Author  : L. Thielemann
# Purpose : Improve Replay of VBOX-Messages
#
LOGFILE=/tmp/mms-vbox.log
{
    vboxfile=${1}
    audiofile=`basename ${2} .wav`
    dir=`dirname ${2}`

    /usr/bin/vboxtoau <${vboxfile} >${dir}/${audiofile}.au
    /usr/bin/sox -v 4.0 -U ${dir}/${audiofile}.au -c 2 -r 44100 -s -w  ${dir}/${audiofile}.wav
} 2>&1 | tee -a ${LOGFILE}
