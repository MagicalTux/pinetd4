#include "ModChargen.hpp"
#include "ModChargenClient.hpp"

ModChargen::ModChargen(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModChargen: new instance");
	pos = 1;
	connect(&t, SIGNAL(timeout()), this, SLOT(makeNextLine()));
	t.setSingleShot(false);
	t.start(100);
}

void ModChargen::incomingTcp(const QString &, QTcpSocket *sock) {
	ModChargenClient *c = new ModChargenClient(sock, this);
	clientAdd(c);
	connect(this, SIGNAL(nextLine(const QByteArray&)), c, SLOT(nextLine(const QByteArray&)));
}

void ModChargen::makeNextLine() {
	QByteArray buf(74,'\0');
	for(int i = 0; i < 72; i++) {
		buf[i] = 32+((i+pos)%95);
	}
	// add CR LF
	buf[73] = '\r';
	buf[74] = '\n';
	pos = (pos + 1) % 95;
	nextLine(buf);
}

