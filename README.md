# pInetd

Fourth evolution of pInetd, this time written in C++ and based on Qt

http://www.pinetd.com

# Installation

## Requirements

* Qt 4.8 or newer (5.0 or newer for Bitcoin and Elastic Search ext)

## Compilation

Compiling is easy.

	qmake
	make

# History

pInetd is initially a PHP daemon allowing to easily write and run network
daemons for various purposes. Since its original version zero as a wrapper
for xinetd, it evolved into a daemon of its own when taking the name pinetd.

pinetd2 goes further by having a more object-oriented structure and processes
for each daemon.

pinetd3 was the first attempt to rewrite pinetd in C by using PHP's engine to
run the daemons, but it failed.

This new evolution aims at solving the previous problems by adopting a fully
modular infrastructure (TODO: insert more bullshit here).

## Major differences compared to pinetd2

First major difference is that the system is now written in C++ and not in
PHP.

The second main difference is that we do not have multiple processes anymore.
Qt's [FSM][fsm] is based on the concept that most actions shouldn't be
blocking, and long work should be run in a QThread (that will send an event on
completion).

Now, the Core class also handles listening on TCP ports. Each time a new
connection happens, the connection is passed to the class currently in charge.
This potentially also allows running multiple threads per group of connections
and makes it possible for the core class to do things (for example enable SSL)
on connections before passing them over.


[fsm]: http://en.wikipedia.org/wiki/Finite-state_machine "Finite-state machine on Wikpiedia"

