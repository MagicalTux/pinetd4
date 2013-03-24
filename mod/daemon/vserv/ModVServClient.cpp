#include "ModVServClient.hpp"
#include "ModVServ.hpp"
#include <QTcpSocket>

ModVServClient::ModVServClient(QTcpSocket *sock, ModVServ *parent): ClientTcp(sock, parent) {
	qDebug("VServ: new client id %s", qPrintable(getId()));
}

void ModVServClient::handleBuffer(const QByteArray &) {
}
