#include "ModBitcoinConnectorClient.hpp"
#include "ModBitcoinConnector.hpp"
#include <QTcpSocket>
#include <QDateTime>
#include <QUuid>
#include <QHostAddress>
#include <QMetaMethod>
#include <QMutex>
#include <ext/BitcoinNetAddr.hpp>
#include <ext/BitcoinCrypto.hpp>
#include <ext/BitcoinStream.hpp>
#include <ext/BitcoinBlock.hpp>

// for details on generation, see
// http://tools.ietf.org/html/rfc864

ModBitcoinConnectorClient::ModBitcoinConnectorClient(QTcpSocket *sock, ModBitcoinConnector *_parent): ClientTcp(sock, _parent) {
	qDebug("BitcoinConnector: new client id %s", qPrintable(getId()));
	parent = _parent;
	bitcoin_version = 209;
	bitcoin_got_version = false;
	bitcoin_sent_version = false;
	is_outgoing = false;
	key = sock->peerAddress().toString()+"/"+QString::number(sock->peerPort());

	connect(&getblocks_timer, SIGNAL(timeout()), this, SLOT(sendGetBlocks()));
	getblocks_timer.setSingleShot(false);
	getblocks_timer.start(300000);
}

ModBitcoinConnectorClient::~ModBitcoinConnectorClient() {
	parent->unregisterPeer(getKey());
	parent->unregisterNonce(bitcoin_nonce); // in case of premature death
}

void ModBitcoinConnectorClient::setOutgoing(bool o) {
	is_outgoing = o;
}

const QString &ModBitcoinConnectorClient::getKey() const {
	return key;
}

void ModBitcoinConnectorClient::bitcoin_packet_addr(const QByteArray&pkt) {
	QDataStream pkt_r(pkt);
	pkt_r.setByteOrder(QDataStream::LittleEndian);

	qint64 count = BitcoinStream::readInt(pkt_r);
	if (pkt.length() < (30*count)) { // TODO: account for length's size (get get_buf_w's position)
		qDebug("incomplete addr, dropped");
		return;
	}

	for(qint64 i = 0; i < count; i++) {
		// we don't really care about timestamp, actually
		quint32 stamp;
		pkt_r >> stamp;
		BitcoinNetAddr a = BitcoinStream::readAddress(pkt_r);
		parent->registerPeer(a, stamp);
	}
}

void ModBitcoinConnectorClient::sendGetBlocks() {
	if (block_todl_list.size() > 0)
		return;
	if (getblocks_timer.interval() != 300000) {
		getblocks_timer.setInterval(300000);
	}
	BitcoinBlock bl = parent->getLastBlock();
	if (!bl.isValid()) {
		// ok, we don't even have the original block, that's bad
		// ORIGIN BLOCK= 6fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d6190000000000
		// Send getdata
		QByteArray buf;
		QDataStream buf_w(&buf, QIODevice::WriteOnly);
		buf_w.setByteOrder(QDataStream::LittleEndian);
		BitcoinStream::writeInt(buf_w, 1);
		buf_w << (quint32)2;
		BitcoinStream::writeData(buf_w, QByteArray::fromHex("6fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d6190000000000"));
		qDebug("sending getdata(%s)", qPrintable(buf.toHex()));
		sendPacket("getdata", buf);
		return;
//		BitcoinStream::writeInt(pkt_w, 1);
//		BitcoinStream::writeData(pkt_w, QByteArray(32, '\0'));
	}

	QByteArray pkt;
	QDataStream pkt_w(&pkt, QIODevice::WriteOnly);
	pkt_w.setByteOrder(QDataStream::LittleEndian);
	pkt_w << (quint32)1;

	QList<QByteArray> info;
	BitcoinBlock cur = bl;
	info.append(bl.getHash());
	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < i; j++) {
			QByteArray p = cur.getParent();
			if (p == QByteArray(32,'\0')) break;
			cur = parent->getBlock(p);
		}
		info.append(cur.getHash());
		if (cur.getParent() == QByteArray(32,'\0')) break;
	}
	BitcoinStream::writeInt(pkt_w, info.length());
	for(int i = 0; i < info.length(); i++)
		BitcoinStream::writeData(pkt_w, info.at(i));

	BitcoinStream::writeData(pkt_w, QByteArray(32, '\0'));
	qDebug("sending getblocks(%s)", qPrintable(pkt.toHex()));
	sendPacket("getblocks", pkt);
}

void ModBitcoinConnectorClient::bitcoin_packet_ping(const QByteArray&pkt) {
	sendPacket("pong", pkt);
}

void ModBitcoinConnectorClient::bitcoin_packet_block(const QByteArray&block) {
	BitcoinBlock bl(block, 0);
	BitcoinBlock pbl = parent->getBlock(bl.getParent());
	if ((!pbl.isValid()) && (bl.getParent() != QByteArray(32,'\0'))) {
		qDebug("Rejecting orphaned block %s (parent: %s)", qPrintable(bl.getHexHash()), qPrintable(bl.getParent().toHex()));
	} else {
		if (pbl.isValid()) bl.setHeight(pbl.getHeight() + 1);
		qDebug("Storing block %s %d", qPrintable(bl.getHexHash()), bl.getHeight());
		if (!parent->knows(2, bl.getHash())) // already know this one
			parent->addBlock(bl);
	}

	while(block_todl_list.size() > 0) {
		QByteArray blid = block_todl_list.takeFirst();
		if (parent->knows(2, blid)) continue;
		QByteArray buf;
		QDataStream buf_w(&buf, QIODevice::WriteOnly);
		buf_w.setByteOrder(QDataStream::LittleEndian);
		BitcoinStream::writeInt(buf_w, 1);
		buf_w << (quint32)2;
		BitcoinStream::writeData(buf_w, blid);
//		qDebug("ModBitcoinConnectorClient::bitcoin_packet_block: sending getdata(%s)", qPrintable(buf.toHex()));
		sendPacket("getdata", buf);
		return;
	}

	getblocks_timer.start(1000);
}

void ModBitcoinConnectorClient::bitcoin_packet_tx(const QByteArray&tx) {
	// compute tx's hash
	QByteArray hash = BitcoinCrypto::doubleSha256(tx);
	if (parent->knows(1, hash)) return; // already know this one
	parent->addInventory(1, hash, tx);
}

void ModBitcoinConnectorClient::bitcoin_packet_getdata(const QByteArray&pkt) {
	QDataStream pkt_r(pkt);
	pkt_r.setByteOrder(QDataStream::LittleEndian);

	qint64 get_count = 0;
	QByteArray get_buf;
	QDataStream get_buf_w(&get_buf, QIODevice::WriteOnly);
	get_buf_w.setByteOrder(QDataStream::LittleEndian);

	qint64 count = BitcoinStream::readInt(pkt_r);
	if (pkt.length() < (36*count)) { // TODO: account for length's size (get get_buf_w's position)
		qDebug("incomplete inv, dropped");
		return; // drop incomplete packet
	}
	for(qint64 i = 0; i < count; i++) {
		quint32 type;
		QByteArray hash;
		pkt_r >> type;
		hash = BitcoinStream::readData(pkt_r, 32);
		if (parent->knows(type, hash)) {
			qDebug("sending data for %d:%s", type, qPrintable(hash.toHex()));
			QByteArray res = parent->getInventory(type, hash);
			switch(type) {
				case 1: sendPacket("tx", res); break;
				case 2: sendPacket("block", res); break;
			}
			continue;
		}

		get_count++;
		get_buf_w << type;
		BitcoinStream::writeData(get_buf_w, hash);
	}

	if (get_count > 0) {
		QByteArray get_buf_len;
		QDataStream get_buf_len_w(&get_buf_len, QIODevice::WriteOnly);
		get_buf_len_w.setByteOrder(QDataStream::LittleEndian);
		BitcoinStream::writeInt(get_buf_len_w, get_count);

		qDebug("sending notfound: %s.%s", qPrintable(get_buf_len.toHex()), qPrintable(get_buf.toHex()));
		sendPacket("notfound", get_buf_len+get_buf);
	}
}

void ModBitcoinConnectorClient::bitcoin_packet_inv(const QByteArray&pkt) {
	QDataStream pkt_r(pkt);
	pkt_r.setByteOrder(QDataStream::LittleEndian);

	qint64 get_count = 0;
	QByteArray get_buf;
	QDataStream get_buf_w(&get_buf, QIODevice::WriteOnly);
	get_buf_w.setByteOrder(QDataStream::LittleEndian);

	qint64 count = BitcoinStream::readInt(pkt_r);
	if (pkt.length() < (36*count)) { // TODO: account for length's size (get get_buf_w's position)
		qDebug("incomplete inv, dropped");
		return; // drop incomplete packet
	}
	for(qint64 i = 0; i < count; i++) {
		quint32 type;
		QByteArray hash;
		pkt_r >> type;
		hash = BitcoinStream::readData(pkt_r, 32);
		if (parent->knows(type, hash)) continue;

		if (type == 2) {
			block_todl_list.append(hash);
			if (block_todl_list.length() > 1)
				continue;
		}

		get_count++;
		get_buf_w << type;
		BitcoinStream::writeData(get_buf_w, hash);
	}

	if (get_count > 0) {
		QByteArray get_buf_len;
		QDataStream get_buf_len_w(&get_buf_len, QIODevice::WriteOnly);
		get_buf_len_w.setByteOrder(QDataStream::LittleEndian);
		BitcoinStream::writeInt(get_buf_len_w, get_count);

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
		QByteArray r_nonce = BitcoinStream::readData(pkt_r, 8);
		if (parent->knownNonce(r_nonce)) {
			qDebug("connected to self by accident");
			sock->close();
			deleteLater();
			return;
		}
		remote_user_agent = BitcoinStream::readString(pkt_r);
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
	getblocks_timer.start(5000);
	
	if (is_outgoing)
		parent->registerPeer(getKey(), remote_services, time(NULL));
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
		QByteArray hashResult = BitcoinCrypto::doubleSha256(pkt_data);
	
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
	BitcoinStream::writeInt(pkt_w, cnt);

	sendPacket("inv", pkt+dat);
}

void ModBitcoinConnectorClient::sendPacket(const QByteArray &type, const QByteArray &data) {
	QByteArray pkt("\xf9\xbe\xb4\xd9", 4); // magic header
	pkt.append(type + QByteArray(12-type.length(), '\0'));

	QDataStream pkt_w(&pkt, QIODevice::WriteOnly);
	pkt_w.setByteOrder(QDataStream::LittleEndian);

	pkt_w.device()->seek(16);

	// compute checksum for data
	QByteArray hashResult = BitcoinCrypto::doubleSha256(data);

	pkt_w << (quint32)data.length();
	BitcoinStream::writeData(pkt_w, hashResult.left(4)); // the checksum
	BitcoinStream::writeData(pkt_w, data);

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

	BitcoinStream::writeAddress(pkt_w, BitcoinNetAddr(sock->peerAddress(), sock->peerPort()));
	BitcoinStream::writeAddress(pkt_w, BitcoinNetAddr::null());
	BitcoinStream::writeData(pkt_w, bitcoin_nonce);
	BitcoinStream::writeString(pkt_w, "/BitcoinConnector:0.1(pinetd4)/");
	pkt_w << (quint32)parent->getBlockHeight();
//	pkt_w << (quint8)1; // relay

	sendPacket("version", pkt);
}

