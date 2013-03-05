#!/bin/sh

exec >symbols.txt

for foo in *.so; do
	nm -f posix "$foo" --defined-only | grep ^_Z | awk '{ print $1 }' | while read bar; do echo "ext/`echo $foo | sed -e 's/^lib//;s/\.so//'` $bar `c++filt $bar`"; done
done
