#include "ModEcho.hpp"
#include "ModEchoClient.hpp"

ModEcho::ModEcho(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModEcho: new instance");
}

void ModEcho::incomingTcp(const QString &, QTcpSocket *sock) {
	clientAdd(new ModEchoClient(sock, this));
}

