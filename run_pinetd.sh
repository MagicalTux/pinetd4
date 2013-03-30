#!/bin/sh
# run pinetd4 in loops without output
exec >/dev/null 2>&1
while true; do
	./pinetd
	sleep 5
done
