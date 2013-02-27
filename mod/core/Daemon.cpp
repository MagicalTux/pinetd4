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
	clients.insert(client->getId(), client);
}

