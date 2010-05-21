#!/usr/bin/env python
#
# Copyright 2004 Anders Rune Jensen
# Distributed under the terms of the GNU General Public License v2

import sys, os, re, urllib

def find_icons(filename):
    matches = []
    last_id = ""
    id_matcher = re.compile('<channel id="(([a-z]|[A-Z]|[0-9]|[:/-])+)')
    url_matcher = re.compile('<icon src="(([a-z]|[A-Z]|[0-9]|[:/.])+)"')
    f=open(filename, 'r')
    lines = f.readlines()
    for line in lines:
	match = id_matcher.search(line)
	if match:
	    last_id = match.group(1)
	match = url_matcher.search(line)
	if match:
	    matches.append([last_id, match.group(1)])
    f.close()
    return matches

def main():
    if len(sys.argv) < 2:
        print "usage: %s [filename]" % os.path.basename(sys.argv[0])
        sys.exit()

    filename = sys.argv[1]

    # generate list of icons to be fetched
    icons = find_icons(filename)

    if not os.access("epg_icons", os.F_OK):
	os.mkdir("epg_icons")

    if len(icons) == 0:
	print "No icons found in", filename
	return

    print "Sucking icons, please wait."

    for i in icons:
	urllib.urlretrieve(i[1], "epg_icons/" + i[0] + ".gif")

    print "Icons downloaded to epg_icons/."
    print "Please move them to your mms configuration directory."

if __name__ == "__main__":
    main()
