#include "ModEchoClient.hpp"
#include "ModEcho.hpp"
#include <QTcpSocket>

ModEchoClient::ModEchoClient(QTcpSocket *sock, ModEcho *parent): ClientTcp(sock, parent) {
	qDebug("client init ok, id %s", qPrintable(getId()));
}

void ModEchoClient::handleBuffer(const QByteArray &buf) {
	sock->write(buf);
}
