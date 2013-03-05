#include "ModBroadcastClient.hpp"
#include "ModBroadcast.hpp"
#include <QTcpSocket>

ModBroadcastClient::ModBroadcastClient(QTcpSocket *sock, ModBroadcast *parent): ClientTcpLine(sock, parent) {
	qDebug("Broadcast: new client id %s", qPrintable(getId()));
}

void ModBroadcastClient::send(const QByteArray &buf) {
	sock->write(buf+"\r\n");
}

void ModBroadcastClient::handleLine(const QByteArray &l) {
	recv(l);
}
