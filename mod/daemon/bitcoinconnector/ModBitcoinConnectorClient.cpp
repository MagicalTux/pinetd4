#include "ModBitcoinConnectorClient.hpp"
#include "ModBitcoinConnector.hpp"
#include <QTcpSocket>
#include <QDateTime>
#include <QUuid>
#include <QHostAddress>
#include <QtCrypto>
#include <QMetaMethod>
#include <QMutex>
#include <ext/BitcoinNetAddr.hpp>

// for details on generation, see
// http://tools.ietf.org/html/rfc864

ModBitcoinConnectorClient::ModBitcoinConnectorClient(QTcpSocket *sock, ModBitcoinConnector *_parent): ClientTcp(sock, _parent) {
	qDebug("BitcoinConnector: new client id %s", qPrintable(getId()));
	parent = _parent;
	bitcoin_version = 209;
	bitcoin_got_version = false;
	bitcoin_sent_version = false;
}

ModBitcoinConnectorClient::~ModBitcoinConnectorClient() {
	parent->unregisterNonce(bitcoin_nonce); // in case of premature death
}

void ModBitcoinConnectorClient::bitcoin_packet_addr(const QByteArray&pkt) {
	QDataStream pkt_r(pkt);
	pkt_r.setByteOrder(QDataStream::LittleEndian);

	qint64 count = readInt(pkt_r);
	if (pkt.length() < (30*count)) { // TODO: account for length's size (get get_buf_w's position)
		qDebug("incomplete addr, dropped");
		return;
	}

	for(qint64 i = 0; i < count; i++) {
	}
}

void ModBitcoinConnectorClient::bitcoin_packet_block(const QByteArray&block) {
	QByteArray hash = doubleSha256(block);
	if (parent->knows(2, hash)) return; // already know this one
	parent->addInventory(2, hash, block);
}

void ModBitcoinConnectorClient::bitcoin_packet_tx(const QByteArray&tx) {
	// compute tx's hash
	QByteArray hash = doubleSha256(tx);
	if (parent->knows(1, hash)) return; // already know this one
	parent->addInventory(1, hash, tx);
}

void ModBitcoinConnectorClient::bitcoin_packet_inv(const QByteArray&pkt) {
	QDataStream pkt_r(pkt);
	pkt_r.setByteOrder(QDataStream::LittleEndian);

	qint64 get_count = 0;
	QByteArray get_buf;
	QDataStream get_buf_w(&get_buf, QIODevice::WriteOnly);
	get_buf_w.setByteOrder(QDataStream::LittleEndian);

	qint64 count = readInt(pkt_r);
	if (pkt.length() < (36*count)) { // TODO: account for length's size (get get_buf_w's position)
		qDebug("incomplete inv, dropped");
		return; // drop incomplete packet
	}
	for(qint64 i = 0; i < count; i++) {
		quint32 type;
		QByteArray hash;
		pkt_r >> type;
		hash = readData(pkt_r, 32);
		if (parent->knows(type, hash)) continue;

		get_count++;
		get_buf_w << type;
		writeData(get_buf_w, hash);
	}

	if (get_count > 0) {
		QByteArray get_buf_len;
		QDataStream get_buf_len_w(&get_buf_len, QIODevice::WriteOnly);
		get_buf_len_w.setByteOrder(QDataStream::LittleEndian);
		writeInt(get_buf_len_w, get_count);

//		qDebug("sending getdata: %s.%s", qPrintable(get_buf_len.toHex()), qPrintable(get_buf.toHex()));
		sendPacket("getdata", get_buf_len+get_buf);
	}
}

void ModBitcoinConnectorClient::bitcoin_packet_version(const QByteArray&pkt) {
	// received version from peer
	QDataStream pkt_r(pkt);
	pkt_r.setByteOrder(QDataStream::LittleEndian);

	pkt_r >> remote_version >> remote_services >> remote_timestamp;
	pkt_r.skipRawData(26); // my addr
	if (!pkt_r.atEnd()) {
		pkt_r.skipRawData(26); // remote addr - use that to register node
		QByteArray r_nonce = readData(pkt_r, 8);
		if (parent->knownNonce(r_nonce)) {
			qDebug("connected to self by accident");
			sock->close();
			deleteLater();
			return;
		}
		remote_user_agent = readString(pkt_r);
		pkt_r >> remote_height;
	}

	// do some work
	bitcoin_got_version = true;
	sendVersion();
	sendPacket("verack", "");
}

void ModBitcoinConnectorClient::bitcoin_packet_verack(const QByteArray&) {
	// no need to keep that nonce anymore
	parent->unregisterNonce(bitcoin_nonce);
}

void ModBitcoinConnectorClient::handleBuffer(const QByteArray &data) {
	in_buf.append(data);

	while(true) {
		if (in_buf.length() < 24) return; // not enough data to do anything

		QByteArray sig("\xf9\xbe\xb4\xd9", 4); // magic header
		if (in_buf.left(4) != sig) {
			qDebug("Got invalid packet from peer, giving up");
			sock->close();
			in_buf.clear();
			deleteLater();
			return;
		}
		QByteArray packet_type = in_buf.mid(4, 12).replace('\0', "");
		QDataStream in_buf_r(&in_buf, QIODevice::ReadOnly);
		in_buf_r.setByteOrder(QDataStream::LittleEndian);
		in_buf_r.device()->seek(12+4); // skip the packet magic & packet name (already read)
		quint32 len;
		in_buf_r >> len;
		// check if we got enough data. Header is 24 bytes long
		if (in_buf.length() < (24+len)) return;
		QByteArray pkt_data = in_buf.mid(24, len);
	
		// compute checksum for data
		QByteArray hashResult = doubleSha256(pkt_data);
	
		if (in_buf.mid(20, 4) != hashResult.left(4)) {
			qDebug("Got invalid hash from peer, giving up");
			sock->close();
			in_buf.clear();
			deleteLater();
			return;
		}
		
		in_buf.remove(0, 24+len);

		int m = metaObject()->indexOfMethod(QByteArray("bitcoin_packet_"+packet_type+"(QByteArray)"));
		if (m == -1) {
			qDebug("No handler for packets of type %s (contents: %s)", qPrintable(packet_type), qPrintable(pkt_data.toHex()));
			continue;
		}
		metaObject()->method(m).invoke(this, Qt::DirectConnection, Q_ARG(const QByteArray &, pkt_data));
	}
}

void ModBitcoinConnectorClient::newInventory(quint32 cnt, const QByteArray&dat) {
	QByteArray pkt;
	QDataStream pkt_w(&pkt, QIODevice::WriteOnly);
	pkt_w.setByteOrder(QDataStream::LittleEndian);
	writeInt(pkt_w, cnt);

	sendPacket("inv", pkt+dat);
}

void ModBitcoinConnectorClient::sendPacket(const QByteArray &type, const QByteArray &data) {
	QByteArray pkt("\xf9\xbe\xb4\xd9", 4); // magic header
	pkt.append(type + QByteArray(12-type.length(), '\0'));

	QDataStream pkt_w(&pkt, QIODevice::WriteOnly);
	pkt_w.setByteOrder(QDataStream::LittleEndian);

	pkt_w.device()->seek(16);

	// compute checksum for data
	QByteArray hashResult = doubleSha256(data);

	pkt_w << (quint32)data.length();
	writeData(pkt_w, hashResult.left(4)); // the checksum
	writeData(pkt_w, data);

//	qDebug("Writing: %s", qPrintable(pkt.toHex()));
	sock->write(pkt);
}

void ModBitcoinConnectorClient::sendVersion() {
	if (bitcoin_sent_version) return;
	bitcoin_sent_version = true;
	quint64 timestamp = QDateTime::currentMSecsSinceEpoch() / 1000;
	qint32 version = 80000;
	quint64 services = 1; // NODE_NETWORK
	bitcoin_nonce = QUuid::createUuid().toRfc4122().right(8);
	parent->registerNonce(bitcoin_nonce);

	QByteArray pkt;
	QDataStream pkt_w(&pkt, QIODevice::WriteOnly);
	pkt_w.setByteOrder(QDataStream::LittleEndian);
	pkt_w << version << services << timestamp;

	writeAddress(pkt_w, BitcoinNetAddr(sock->peerAddress(), sock->peerPort()));
	writeAddress(pkt_w, BitcoinNetAddr::null());
	writeData(pkt_w, bitcoin_nonce);
	writeString(pkt_w, "/BitcoinConnector:0.1(pinetd4)/");
	pkt_w << (quint32)parent->getBlockHeight();
//	pkt_w << (quint8)1; // relay

	sendPacket("version", pkt);
}

void ModBitcoinConnectorClient::writeAddress(QDataStream &stream, const BitcoinNetAddr &addr) {
	writeData(stream, addr.getBin());
}

void ModBitcoinConnectorClient::writeString(QDataStream &stream, const QByteArray &string) {
	writeInt(stream, string.length());
	writeData(stream, string);
}

void ModBitcoinConnectorClient::writeInt(QDataStream &stream, quint64 i) {
	if (i < 253) {
		stream << (quint8)i;
		return;
	}
	if (i < 0xffff) {
		stream << (quint8)253 << (quint16)i;
		return;
	}
	if (i < 0xffffffff) {
		stream << (quint8)254 << (quint32)i;
		return;
	}
	stream << (quint8)255 << i;
}

void ModBitcoinConnectorClient::writeData(QDataStream &stream, const QByteArray &data) {
	stream.writeRawData(data.constData(), data.length());
}

quint64 ModBitcoinConnectorClient::readInt(QDataStream &stream) {
	quint8 i;
	stream >> i;
	switch(i) {
		case 253:
			{
				quint16 j;
				stream >> j;
				return j;
			}
		case 254:
			{
				quint32 j;
				stream >> j;
				return j;
			}
		case 255:
			{
				quint64 j;
				stream >> j;
				return j;
			}
		default:
			return i;
	}
}

QByteArray ModBitcoinConnectorClient::readString(QDataStream &stream) {
	qint64 len = readInt(stream);
	return readData(stream, len);
}

QByteArray ModBitcoinConnectorClient::readData(QDataStream &stream, int len) {
	QByteArray final(len, '\0');
	int res = stream.readRawData(final.data(), len);
	if (res < len) final.truncate(res);
	return final;
}

QByteArray ModBitcoinConnectorClient::doubleSha256(const QByteArray &input) {
	// compute checksum for data
	// Do not instanciate a sha object each time for faster processing
	static QMutex shamutex;
	shamutex.lock();
	static QCA::Hash sha("sha256");
	sha.clear();
	sha.update(input);
	QByteArray hashResult = sha.final().toByteArray();
	sha.clear();
	sha.update(hashResult);
	hashResult = sha.final().toByteArray();
	shamutex.unlock();
	return hashResult;
}

