#include "ModRConnect.hpp"
#include "ModRConnectClient.hpp"

ModRConnect::ModRConnect(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModRConnect: new instance");
}

void ModRConnect::incomingTcp(const QString &, QTcpSocket *sock) {
	clientAdd(new ModRConnectClient(sock, this));
}

