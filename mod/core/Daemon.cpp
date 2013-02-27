#include <mod/core/Daemon.hpp>
#include <mod/core/Client.hpp>
#include <QTcpSocket>

Daemon::Daemon(const QString &_modname, const QString &_instname) {
	modname = _modname;
	instname = _instname;
}

void Daemon::incomingTcp(const QString &, QTcpSocket *sock) {
	qDebug("Daemon: got incoming TCP connection without handler, closing it");
	sock->close();
	sock->deleteLater();
}

void Daemon::clientAdd(Client *client) {
	connect(client, SIGNAL(destroyed(QObject*)), this, SLOT(clientLost(QObject*)));
	clients.insert(client->getId(), client);
}

void Daemon::clientLost(QObject *obj) {
	Client *c = (Client*)obj;
	QString id = c->getId();
	qDebug("Daemon: lost client id %s", qPrintable(id));
	clients.remove(id);
}

