#!/bin/sh

version=""

if test -d .bzr; then version=`bzr version-info | grep revno` ; fi

echo "#define VERSION \"${version}\"" > version.h
