TEMPLATE = subdirs
contains(gfx-plugins, ahi)          :SUBDIRS += ahi
contains(gfx-plugins, directfb)     :SUBDIRS += directfb
contains(gfx-plugins, linuxfb)      :SUBDIRS += linuxfb
contains(gfx-plugins, qvfb)	    :SUBDIRS += qvfb
contains(gfx-plugins, vnc)	    :SUBDIRS += vnc
contains(gfx-plugins, transformed)  :SUBDIRS += transformed
contains(gfx-plugins, svgalib)      :SUBDIRS += svgalib
contains(gfx-plugins, powervr)      :SUBDIRS += powervr
contains(gfx-plugins, rtd)          :SUBDIRS += rtd

