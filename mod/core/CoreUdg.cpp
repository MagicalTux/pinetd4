#include <core/CoreUdg.hpp>
#include <core/Core.hpp>
#include <core/Daemon.hpp>
#include <core/QUnixDatagramServer.hpp>

CoreUdg::CoreUdg(const QString &socket, Core *_parent): QObject(_parent) {
	parent = _parent;
	server = new QUnixDatagramServer(socket, this);
	connect(server, SIGNAL(message(const QByteArray&)), this, SLOT(message(const QByteArray&)));
	receiver = NULL;
}

bool CoreUdg::isValid() const {
	return server->isValid();
}

void CoreUdg::setTarget(QObject *target, const QString &_entry) {
	if (receiver == target) {
		entry = _entry;
		return; // no change
	}
	if (receiver != NULL) {
		disconnect(this, SIGNAL(outgoingDatagram(const QByteArray&, DatagramReply*)), receiver, SLOT(incomingDatagram(const QByteArray&, DatagramReply*)));
		disconnect(receiver, SIGNAL(destroyed(QObject*)), this, SLOT(targetDestroyed(QObject*)));
		receiver = NULL;
	}

	receiver = target;
	entry = _entry;
	if (receiver == NULL) return; // disconnect

	connect(this, SIGNAL(outgoingDatagram(const QByteArray&, DatagramReply*)), receiver, SLOT(incomingDatagram(const QByteArray&, DatagramReply*)));
	connect(receiver, SIGNAL(destroyed(QObject*)), this, SLOT(targetDestroyed(QObject*)));
}

void CoreUdg::targetDestroyed(QObject *o) {
	if (o != receiver) return;
	setTarget(NULL);
}

void CoreUdg::message(const QByteArray &msg) {
	outgoingDatagram(msg, NULL);
}

