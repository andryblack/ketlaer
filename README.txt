Build environment:
==================

Use the toolchain named toolchain_mipsel-6.03-6.i586.rpm from 
the asus download (http://support.asus.com/download/download.aspx?product=19&model=O!Play%20HDP-R1&SLanguage=en-en&os=5).

For Debian-based systems (YMMV): alien fails to create a .deb but does extract working files to ./toolchain_mipsel-6.03/usr/local/toolchain_mipsel.
Copy, move or symlink this to /usr/local/toolchain_mipsel and add /usr/local/toolchain_mipsel/bin to your PATH.

Extract the toolchain fix (utility/toolchain_fix) in the toolchain lib directory.

Set the environment KETLAER to /usr/local/etc/ketlaer.
Set the environment KETLAER_DIR to the absolute path 
of your ketlaer source dir.
Extend PATH to point to /usr/local/bin.

Ketlaer is intended to be build to /usr/local/etc/ketlaer. 
It is not recommended to change this at the moment.

Building:
=========

Most directories provide a ketlaer-build build script.

Build order:

utility/config
utility/libm2
libketlaer

(Now you can build TESTClient, if you are in a hurry)

utility/zlib-1.2.5
utility/libpng-1.2.29
utility/freetype-2.3.7
utility/jpeg-8b
utility/imlib2-1.4.0 (imlib2_conv does not build. you can ignore this.)
utility/libiconv-1.13.1
utility/gettext-0.18 (fix gettext-tools/gnulib-lib/spawn.h adding #include <sched.h>)
utility/ncurses-5.7
utility/commoncpp2-1.5.2
utility/sqlite-3.6.23.1
utility/Python-2.6.5
utility/libxml2-2.7.7
utility/pcre-8.02
utility/taglib-1.6.3
mms-1.1.1

Download qt-everywhere-opensource-src-4.6.2 sources.
Unpack them in the utility directory. 
Overlay them with the ketlaer files.

utility/qt-everywhere-opensource-src-4.6.2
utility/sip-4.10.2
utility/pyqt-4.7.3

IMPORTANT:

After that copy all *.so* from the toolchain lib directory
to $KETLAER/lib with cp -d !!

Runtime enviroment (on realtek):
================================

Make a symbolic link /usr/local/etc/ketlaer to point to a r/w mounted usb-drive 
with the ketlaer build or a samba mount of the builded /usr/local/etc/ketlaer.  

Set the following environment:

export KETLAER=/usr/local/etc/ketlaer

export PATH=$KETLAER/bin:$KETLAER/python/bin:$PATH
export LD_LIBRARY_PATH=$KETLAER/lib

export QWS_MOUSE_PROTO="usb"
export QWS_KEYBOARD="usb"

Starting mms:
=============

Add at least 64MB swapspace!
Copy configs from mms dir to $KETLAER/etc/mms.
Edit configs, using rtd as outputdevice, and rtd as AudioDevice.

Run stopall.
Run it with mms -u root -p /path_to_cache_dir_read_write

Starting a Qt program:
======================

Run stopall.

Choose one from the examples or demos directory.
Run it with ./program -qws -display DISPLAY, where DISPLAY 
is one of the following:

vnc	  (for vnc output)
rtd	  (for tv/hdmi output)
vnc:rtd	  (for both. colors will be wrong on vnc.)

Starting a PyQt4 program:
=========================

Run stopall.

Run it with python program.py -qws -display DISPLAY. See above
for the possible values of DISPLAY.

WebKit:
=======

Run examples/webkit/fancybrowser.


BUGS:
=====

- Qt will crash on shutdown of program. Seems to be related to toolchain.
- mms also. Have to fix toolchain someday.
- Lots of others still to discover :-)
