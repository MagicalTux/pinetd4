#include "ModChargenClient.hpp"
#include "ModChargen.hpp"
#include <QTcpSocket>

ModChargenClient::ModChargenClient(QTcpSocket *sock, ModChargen *parent): ClientTcp(sock, parent) {
	qDebug("Chargen: new client id %s", qPrintable(getId()));
	pos = 1;
	connect(&t, SIGNAL(timeout()), this, SLOT(nextLine()));
	t.setSingleShot(false);
	t.start(100);
}

void ModChargenClient::nextLine() {
	QByteArray buf(73,'\0');
	for(int i = 0; i < 72; i++) {
		buf[i] = 32+((i+pos)%95);
	}
	buf[73] = '\n';
	pos = (pos + 1) % 95;
	sock->write(buf);
}

void ModChargenClient::handleBuffer(const QByteArray &) {
}
