#include <core/Client.hpp>
#include <core/Daemon.hpp>
#include <QUuid>

Client::Client(Daemon *_parent): QObject(_parent) {
	parent = _parent;
	id = QUuid::createUuid().toString().mid(1, 36).toLower();
}

Client::~Client() {
	// bye bye
	qDebug("client dies");
}

const QString &Client::getId() const {
	return id;
}

