#! /bin/sh

# make distclean
rm -rf `find . -name .libs`
rm -rf `find . -name .deps`
rm -rf `find . -name \*.core`
rm -rf `find . -name autom4te.cache`
rm -rf `find . -name \*~`
rm -rf `find . -name .\#*`