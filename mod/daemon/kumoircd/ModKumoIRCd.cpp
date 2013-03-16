#include "ModKumoIRCd.hpp"
#include "ModKumoIRCdClient.hpp"

ModKumoIRCd::ModKumoIRCd(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModKumoIRCd: new instance");
}

void ModKumoIRCd::incomingTcp(const QString &, QTcpSocket *sock) {
	clientAdd(new ModKumoIRCdClient(sock, this));
}

QByteArray ModKumoIRCd::getServerName() {
	return "localhost";
}

