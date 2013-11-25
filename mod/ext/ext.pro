TEMPLATE = subdirs
SUBDIRS = tcpline bitcoin
contains(QT_VERSION, ^5.*) {
	SUBDIRS += elasticsearch
}
