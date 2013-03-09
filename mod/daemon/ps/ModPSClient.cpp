#include "ModPSClient.hpp"
#include "ModPS.hpp"
#include <QTcpSocket>

ModPSClient::ModPSClient(QTcpSocket *sock, ModPS *parent): ClientTcp(sock, parent) {
	qDebug("PS: new client id %s", qPrintable(getId()));
}

void ModPSClient::handleBuffer(const QByteArray &) {
}
