#include "ModChargenClient.hpp"
#include "ModChargen.hpp"
#include <QTcpSocket>

// for details on generation, see
// http://tools.ietf.org/html/rfc864

ModChargenClient::ModChargenClient(QTcpSocket *sock, ModChargen *parent): ClientTcp(sock, parent) {
	qDebug("Chargen: new client id %s", qPrintable(getId()));
	pos = 1;
	connect(&t, SIGNAL(timeout()), this, SLOT(nextLine()));
	t.setSingleShot(false);
	t.start(100);
}

void ModChargenClient::nextLine() {
	QByteArray buf(74,'\0');
	for(int i = 0; i < 72; i++) {
		buf[i] = 32+((i+pos)%95);
	}
	// add CR LF
	buf[73] = '\r';
	buf[74] = '\n';
	pos = (pos + 1) % 95;
	sock->write(buf);
}

void ModChargenClient::handleBuffer(const QByteArray &) {
}
