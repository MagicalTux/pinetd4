#include <core/CoreTcp.hpp>
#include <core/Core.hpp>
#include <core/Daemon.hpp>
#include <QTcpSocket>
#include <QStringList>

CoreTcp::CoreTcp(Core *_parent): QTcpServer(_parent) {
	parent = _parent;
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
	QTcpSocket *s = new QTcpSocket(d);
	s->setSocketDescriptor(socketDescriptor);
	if (d == NULL) {
		s->close();
		delete s;
		return;
	}
	d->incomingTcp(entry, s);
}

