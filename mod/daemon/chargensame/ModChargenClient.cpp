#include "ModChargenClient.hpp"
#include "ModChargen.hpp"
#include <QTcpSocket>

// for details on generation, see
// http://tools.ietf.org/html/rfc864

ModChargenClient::ModChargenClient(QTcpSocket *sock, ModChargen *parent): ClientTcp(sock, parent) {
	qDebug("Chargen: new client id %s", qPrintable(getId()));
}

void ModChargenClient::nextLine(const QByteArray &buf) {
	sock->write(buf);
}

void ModChargenClient::handleBuffer(const QByteArray &) {
}
