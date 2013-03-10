#include "ModPS.hpp"
#include "ModPSClient.hpp"

ModPS::ModPS(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModPS: new instance");
}

void ModPS::incomingTcp(const QString &, QTcpSocket *sock) {
	clientAdd(new ModPSClient(sock, this));
}

void ModPS::incomingDatagram(const QByteArray&dat, DatagramReply*) {
	qDebug("ModPS: got datagram");
}

