#!/bin/sh

exec >symbols.txt

for foo in *.so; do
	nm -f posix libtcpline.so --defined-only | grep ^_Z | awk '{ print $1 }' | while read bar; do echo "$foo $bar `c++filt $bar`"; done
done
