TEMPLATE = subdirs
SUBDIRS = tcpline qamqp
contains(QT_VERSION, ^5.*) {
	SUBDIRS += bitcoin elasticsearch
}
