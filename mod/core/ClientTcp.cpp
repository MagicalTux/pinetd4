#include <core/ClientTcp.hpp>
#include <QTcpSocket>

ClientTcp::ClientTcp(QTcpSocket *_sock, Daemon *_parent): Client(_parent) {
	sock = _sock;
	connect(sock, SIGNAL(readyRead()), this, SLOT(doRead()));
	connect(sock, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
	connect(sock, SIGNAL(disconnected()), this, SLOT(deleteLater()));
	connect(sock, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
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

void ClientTcp::write(const QByteArray &dat) {
	if (write_buf.isEmpty()) {
		qint64 s = sock->write(dat);
		if (s < 0) {
			qDebug("ClientTcp::write: error while writing, dropping link");
			sock->close();
			deleteLater();
			return;
		}
		if (s == dat.size()) return;
		write_buf.append(dat.mid(s));
		return;
	}
	write_buf.append(dat);

	if (write_buf.size() > (1024*1024)) {
		// buffer too big!
		qDebug("ClientTcp::write: write buffer over 1MB, dropping link");
		write_buf.clear();
		sock->close();
		deleteLater();
	}
}

void ClientTcp::flush() {
	if (write_buf.isEmpty()) return;
	qint64 s = sock->write(write_buf);
	if (s < 0) {
		qDebug("ClientTcp::flush: error while writing, dropping link");
		sock->close();
		deleteLater();
		return;
	}
	write_buf.remove(0, s);
}

void ClientTcp::bytesWritten(qint64) {
	flush();
}

