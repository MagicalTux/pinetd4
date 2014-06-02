TEMPLATE = subdirs
SUBDIRS = echo chargen chargensame broadcast ps kumoircd vserv midasinput rconnect
contains(QT_VERSION, ^5.*) {
	SUBDIRS += bitcoinconnector vps
}
