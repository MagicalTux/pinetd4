#include <core/CoreTcp.hpp>
#include <core/Core.hpp>
#include <core/Daemon.hpp>
#include <QTcpSocket>
#include <QStringList>
#include <QMetaObject>

CoreTcp::CoreTcp(Core *_parent): QTcpServer(_parent) {
	parent = _parent;
	receiver = NULL;
	// register QTcpSocket* type, just in case
//	qRegisterMetaType<QTcpSocket*>("QTcpSocket*");
}

void CoreTcp::setTarget(QObject *target, const QString &_entry) {
	if (receiver == target) {
		entry = _entry;
		return; // no change
	}
	if (receiver != NULL) {
		disconnect(this, SIGNAL(outgoingTcp(const QString&, QTcpSocket*)), receiver, SLOT(incomingTcp(const QString&, QTcpSocket*)));
		disconnect(receiver, SIGNAL(destroyed(QObject*)), this, SLOT(targetDestroyed(QObject*)));
		receiver = NULL;
	}

	receiver = target;
	entry = _entry;
	if (receiver == NULL) return; // disconnect

	connect(this, SIGNAL(outgoingTcp(const QString&, QTcpSocket*)), receiver, SLOT(incomingTcp(const QString&, QTcpSocket*)));
	connect(receiver, SIGNAL(destroyed(QObject*)), this, SLOT(targetDestroyed(QObject*)));
}

void CoreTcp::targetDestroyed(QObject *o) {
	if (o != receiver) return;
	setTarget(NULL);
}

void CoreTcp::incomingConnection(int socketDescriptor) {
	QTcpSocket *s = new QTcpSocket();
	s->setSocketDescriptor(socketDescriptor);
	if (receiver == NULL) {
		s->close();
		delete s;
		return;
	}
	s->moveToThread(receiver->thread());
	s->setParent(receiver);

	outgoingTcp(entry, s);
}

