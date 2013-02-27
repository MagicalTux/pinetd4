#include <mod/core/ClientTcp.hpp>
#include <QTcpSocket>

ClientTcp::ClientTcp(QTcpSocket *_sock, Daemon *_parent): Client(_parent) {
	sock = _sock;
	connect(sock, SIGNAL(readyRead()), this, SLOT(doRead()));
	connect(sock, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
	connect(sock, SIGNAL(disconnected()), this, SLOT(deleteLater()));
	connect(this, SIGNAL(destroyed(QObject*)), sock, SLOT(deleteLater()));
}

void ClientTcp::doRead() {
	qint64 to_read = sock->bytesAvailable();
	if (to_read > 1024*1024) to_read = 1024*1024; // limit to 1MB per read
	QByteArray buf(to_read, '\0');
	qint64 dat = sock->read(buf.data(), buf.size());
	if (dat == 0) return; // no data, actually (should it happen?)
	if (dat == -1) {
		// some kind of error
		sock->close();
		deleteLater();
		return;
	}

	if (dat < to_read) {
		buf.truncate(dat);
	}

	handleBuffer(buf);
}

