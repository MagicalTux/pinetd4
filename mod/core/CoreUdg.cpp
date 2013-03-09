#include <core/CoreUdg.hpp>
#include <core/Core.hpp>
#include <core/Daemon.hpp>
#include <QStringList>
#include <QMetaObject>
#include <core/QUnixDatagramServer.hpp>

CoreUdg::CoreUdg(const QString &socket, Core *_parent): QObject(_parent) {
	parent = _parent;
	server = new QUnixDatagramServer(socket, this);
	connect(server, SIGNAL(message(const QByteArray&)), this, SLOT(message(const QByteArray&)));
}

bool CoreUdg::isValid() const {
	return server->isValid();
}

void CoreUdg::setTarget(const QString &t) {
	QStringList sub = t.split(":");
	target = sub.at(0);
	if (sub.size() >= 2) {
		entry = sub.at(1);
	} else {
		entry = "main";
	}
}

void CoreUdg::message(const QByteArray &msg) {
	Daemon *d = parent->getDaemon(target);
	if (d == NULL) return;
	QMetaObject::invokeMethod(d, "incomingDatagram", Q_ARG(const QByteArray&, msg));
}

