#!/bin/sh

if [ -e /sys/class/rtc/rtc0/wakealarm ]; then
	SECS=`date -d " $@" "+%s" -u`
        echo 0 > /sys/class/rtc/rtc0/wakealarm
        echo $SECS > /sys/class/rtc/rtc0/wakealarm
fi
if [ -e /proc/acpi/alarm ]; then
        echo "$@" > /proc/acpi/alarm
fi
