TEMPLATE = subdirs
SUBDIRS = echo chargen chargensame broadcast ps kumoircd bitcoinconnector vserv midasinput
contains(QT_VERSION, ^5.*) {
	SUBDIRS += vps
}
