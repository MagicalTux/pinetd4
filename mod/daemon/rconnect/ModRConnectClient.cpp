#include "ModRConnectClient.hpp"
#include "ModRConnect.hpp"
#include <QTcpSocket>
#include <QDataStream>

static QMap<QByteArray,ModRConnectClient*> rconnect;

ModRConnectClient::ModRConnectClient(QTcpSocket *sock, ModRConnect *parent): ClientTcp(sock, parent) {
	qDebug("RConnect: new client id %s", qPrintable(getId()));

	raw_id = QByteArray::fromHex(getId().toLatin1());

	// register client
	rconnect.insert(raw_id, this);

	// send client id
	write(raw_id); // 16 bytes always
}

ModRConnectClient::~ModRConnectClient() {
	rconnect.remove(raw_id);
}

void ModRConnectClient::handleBuffer(const QByteArray &buf) {
	// Buffer format: <16 bytes target id><2~4 bytes length><data>
	buf_in.append(buf);
	if (buf_in.length() < 18) {
		return; // not enough to be useful
	}
	QByteArray tgt = buf_in.mid(0, 16);
	QDataStream r(buf_in);
	r.skipRawData(16);
	quint32 len;
	quint16 len_min;
	quint32 header_len = 18;

	r >> len_min;
	len = len_min;
	if (len_min & 0x8000) {
		if (buf_in.length() < 20) {
			return;
		}
		r >> len_min;
		len = ((len & 0x7fff) << 16) | len_min;
		header_len = 20;
	}
	if ((quint64)buf_in.length() < len+header_len) {
		return; // not enough yet
	}

	// got enough data, send to target
	if (!rconnect.contains(tgt)) {
		// skip
		qDebug("Skipping packet to unknown target");
		buf_in.remove(0, len+header_len);
		return;
	}

	// send
	ModRConnectClient *tgt_c = rconnect.value(tgt);
//	qDebug("sending data from %s to %s", qPrintable(getId()), qPrintable(tgt_c->getId()));
	tgt_c->write(raw_id + buf_in.mid(16, len + header_len - 16));
	buf_in.remove(0, len+header_len);
}

