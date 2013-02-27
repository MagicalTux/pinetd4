#include "ModChargen.hpp"
#include "ModChargenClient.hpp"

ModChargen::ModChargen(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModChargen: new instance");
}

void ModChargen::incomingTcp(const QString &, QTcpSocket *sock) {
	clientAdd(new ModChargenClient(sock, this));
}

