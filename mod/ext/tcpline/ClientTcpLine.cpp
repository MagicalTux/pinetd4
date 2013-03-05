#include <ext/ClientTcpLine.hpp>
#include <QTcpSocket>

ClientTcpLine::ClientTcpLine(QTcpSocket *sock, Daemon *parent): ClientTcp(sock, parent) {
}

void ClientTcpLine::handleBuffer(const QByteArray&buf) {
	read_buffer.append(buf);

	while(1) {
		int p = read_buffer.indexOf('\n');
		if (p == -1) {
			if (read_buffer.size() > 4096) {
				// that line is way too long
				read_buffer.clear();
				sock->close();
			}
			return;
		}
		if (p > 4096) {
			// that line is way too long
			read_buffer.clear();
			sock->close();
			return;
		}

		QByteArray line = read_buffer.left(p);
		read_buffer.remove(0, p+1);

		handleLine(line.trimmed());
	}
}

