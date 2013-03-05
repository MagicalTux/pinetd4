#include "ModBroadcast.hpp"
#include "ModBroadcastClient.hpp"

ModBroadcast::ModBroadcast(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModBroadcast: new instance");
}

void ModBroadcast::incomingTcp(const QString &, QTcpSocket *sock) {
	ModBroadcastClient *c = new ModBroadcastClient(sock, this);
	clientAdd(c);
	connect(c, SIGNAL(recv(const QByteArray&)), this, SLOT(incoming(const QByteArray&)));
	connect(this, SIGNAL(broadcast(const QByteArray&)), c, SLOT(send(const QByteArray&)));
}

void ModBroadcast::incoming(const QByteArray&m) {
	broadcast(m);
}

