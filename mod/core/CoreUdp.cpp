#include <core/CoreUdp.hpp>
#include <core/Core.hpp>
#include <core/Daemon.hpp>

CoreUdp::CoreUdp(Core *_parent): QUdpSocket(_parent) {
	parent = _parent;
	connect(this, SIGNAL(readyRead()), this, SLOT(receiveDatagram()));
	receiver = NULL;
}

void CoreUdp::receiveDatagram() {
	while(hasPendingDatagrams()) {
		QByteArray data(pendingDatagramSize(), '\0');
		QHostAddress addr;
		quint16 port;

		qint64 len = readDatagram(data.data(), pendingDatagramSize(), &addr, &port);
		if (len != data.length()) data.resize(len); // shouldn't happen

		outgoingDatagram(data, NULL); // TODO: create a reply object with addr & port
	}
}

void CoreUdp::setTarget(QObject *target, const QString &_entry) {
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

void CoreUdp::targetDestroyed(QObject *o) {
	if (o != receiver) return;
	setTarget(NULL);
}

