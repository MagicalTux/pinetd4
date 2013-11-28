TEMPLATE = subdirs
SUBDIRS = tcpline
contains(QT_VERSION, ^5.*) {
	SUBDIRS += bitcoin elasticsearch
}
