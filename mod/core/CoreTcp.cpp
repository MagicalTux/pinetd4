#include <core/CoreTcp.hpp>
#include <core/Core.hpp>
#include <core/Daemon.hpp>
#include <QTcpSocket>
#include <QStringList>
#include <QMetaObject>

CoreTcp::CoreTcp(Core *_parent): QTcpServer(_parent) {
	parent = _parent;
	// register QTcpSocket* type, just in case
	qRegisterMetaType<QTcpSocket*>("QTcpSocket*");
}

void CoreTcp::setTarget(const QString &t) {
	QStringList sub = t.split(":");
	target = sub.at(0);
	if (sub.size() >= 2) {
		entry = sub.at(1);
	} else {
		entry = "main";
	}
}

void CoreTcp::incomingConnection(int socketDescriptor) {
	Daemon *d = parent->getDaemon(target);
	QTcpSocket *s = new QTcpSocket();
	s->setSocketDescriptor(socketDescriptor);
	if (d == NULL) {
		s->close();
		delete s;
		return;
	}
	s->moveToThread(d->thread());
	s->setParent(d);
	QMetaObject::invokeMethod(d, "incomingTcp", Q_ARG(QString, entry), Q_ARG(QTcpSocket*, s));
}

