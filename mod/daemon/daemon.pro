TEMPLATE = subdirs
SUBDIRS = echo chargen chargensame broadcast ps kumoircd

exists(bitcoinconnector/Makefile) {
	SUBDIRS += bitcoinconnector
}
