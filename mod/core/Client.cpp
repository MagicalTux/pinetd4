#include <mod/core/Client.hpp>
#include <mod/core/Daemon.hpp>
#include <QUuid>

Client::Client(Daemon *_parent): QObject(_parent) {
	parent = _parent;
	id = QUuid::createUuid().toString().mid(1, 36).toLower();
}

Client::~Client() {
	// bye bye
}

const QString &Client::getId() const {
	return id;
}

