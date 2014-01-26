#include "ModBitcoinConnector.hpp"
#include "ModBitcoinConnectorClient.hpp"
#include <QStringList>
#include <QVariant>
#include <QTcpSocket>
#include <ext/BitcoinNetAddr.hpp>
#include <ext/BitcoinBlock.hpp>
#include <QJsonArray>

#define SQL_QUERY(_var, _req)
#define SQL_BIND(_query, _var, _val)

ModBitcoinConnector::ModBitcoinConnector(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModBitcoinConnector: new instance");
	db_open = false;
	inventory_queue_timer.setSingleShot(false);
	inventory_queue_timer.start(10000);
	connect(&inventory_queue_timer, SIGNAL(timeout()), this, SLOT(sendQueuedInventory()));
}

const QByteArray &ModBitcoinConnector::getClientId() const {
	return client_id;
}

quint32 ModBitcoinConnector::getBlockHeight() {
	// TODO store this value in cache for fast retriveral
	
	if (!db_open) return 0;

	// make things [faster/easier to read] by passing json string directly (static search)
	QJsonObject stat = db->search("{\"query\":{\"match_all\":{}},\"facets\":{\"stat1\":{\"statistical\":{\"field\":\"block.depth\"}}}}", "bitcoin/block");

	return stat.value("facets").toObject().value("stat1").toObject().value("max").toDouble();
}

BitcoinBlock ModBitcoinConnector::getLastBlock() {
	quint32 block_height = getBlockHeight();
	QJsonObject term_obj;
	term_obj.insert("depth", QJsonValue((qint64)block_height));
	QJsonObject query_obj;
	query_obj.insert("term", term_obj);
	QJsonObject body_obj;
	body_obj.insert("query", query_obj);
	QJsonObject res = db->search(body_obj, "bitcoin/block");

	QJsonObject hit = res.value("hits").toObject().value("hits").toArray().at(0).toObject().value("_source").toObject();

	if (!hit.contains("raw_header")) return BitcoinBlock();

	return BitcoinBlock(QByteArray::fromBase64(hit.value("raw_header").toString().toLatin1()), hit.value("depth").toDouble());
}

BitcoinBlock ModBitcoinConnector::getBlock(const QByteArray &hash) {
	QJsonObject hit = db->get(hash.toHex(), "bitcoin", "block");

	if (!hit.contains("raw_header")) return BitcoinBlock();

	return BitcoinBlock(QByteArray::fromBase64(hit.value("raw_header").toString().toLatin1()), hit.value("depth").toDouble());
}

QByteArray ModBitcoinConnector::getInventory(quint32 type, const QByteArray &hash) {
	QByteArray key;
	QDataStream s(&key, QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << type;
	key.append(hash);

	if (inventory_cache.contains(key)) return *inventory_cache.object(key);

	if (type == 1) {
		QJsonObject obj = db->get(hash.toHex(), "bitcoin", "tx");
		if (!obj.contains("raw_data")) return QByteArray();
		QByteArray res = QByteArray::fromBase64(obj.value("raw_data").toString().toLatin1());
		inventory_cache.insert(key, new QByteArray(res));
		return res;
	}
#if 0
	if (type == 2) {
		SQL_QUERY(query, "SELECT data FROM blocks WHERE hash = :hash");
		SQL_BIND(query, hash, hash);
		if (!query.exec()) {
			return QByteArray(); // error
		}
		if (!query.next()) {
			return QByteArray(); // wat?
		}
		QByteArray res = query.value(0).toByteArray();
		inventory_cache.insert(key, new QByteArray(res));
		return res;
	}
#endif
	return QByteArray();
}

bool ModBitcoinConnector::knows(quint32 type, const QByteArray &hash) {
	QByteArray key;
	QDataStream s(&key, QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << type;
	key.append(hash);

//	qDebug("ModBitcoinConnector: do I know %d:%s?", type, qPrintable(hash.toHex()));
	if (inventory_cache.contains(key)) return true;

	if (type == 1) {
		return db->contains(hash.toHex(), "bitcoin", "tx");
	}
	if (type == 2) {
		return db->contains(hash.toHex(), "bitcoin", "block");
	}
	return false; // wtf?
}

void ModBitcoinConnector::addInventory(quint32 type, const QByteArray &hash, const QByteArray &data, bool send_inv) {
	QByteArray key;
	QDataStream s(&key, QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << type;
	key.append(hash);

//	qDebug("ModBitcoinConnector: recording %d:%s", type, qPrintable(hash.toHex()));
	inventory_cache.insert(key, new QByteArray(data));
	if (send_inv)
		inventory_queue.append(key);

	if (type == 1) {
		// TODO: parse tx_in and tx_out and input in elasticsearch
		BitcoinTx tx(data);
		QList<BitcoinTxIn> tx_in = tx.txIn();
		for(int i = 0; i < tx_in.length(); i++) {
			QJsonObject tx_in_obj;
			tx_in_obj.insert("hash_n", QString(hash.toHex()+":%1").arg(i));
			tx_in_obj.insert("hash", QString(hash.toHex()));
			tx_in_obj.insert("n", i);
			tx_in_obj.insert("prev_out_hash", QString(tx_in.at(i).getPrevOutHash().toHex()));
			tx_in_obj.insert("prev_out_n", (qint64)tx_in.at(i).getPrevOutIndex());
			db->index(tx_in_obj, "bitcoin", "tx_in");
		}
		QList<BitcoinTxOut> tx_out = tx.txOut();
		for(int i = 0; i < tx_out.length(); i++) {
			QJsonObject tx_out_obj;
			tx_out_obj.insert("hash_n", QString(hash.toHex()+":%1").arg(i));
			tx_out_obj.insert("hash", QString(hash.toHex()));
			tx_out_obj.insert("n", i);
			tx_out_obj.insert("value", (qint64)tx_out.at(i).getValue());
//			tx_out_obj.insert("scriptpubkey", );
			tx_out_obj.insert("claimed", QJsonValue(false));
			tx_out_obj.insert("addr", QString(tx_out.at(i).getTxOutAddr().toHex()));
			db->index(tx_out_obj, "bitcoin", "tx_out");
		}
		QJsonObject tx_obj;
		tx_obj.insert("hash", QJsonValue(QString(hash.toHex())));
		tx_obj.insert("version", QJsonValue((qint64)tx.getVersion()));
		tx_obj.insert("lock_time", QJsonValue((qint64)tx.getLockTime()));
		tx_obj.insert("size", QJsonValue(data.length()));
		tx_obj.insert("raw_data", QJsonValue(QString(data.toBase64())));
		db->index(tx_obj, "bitcoin", "tx");
		qDebug("BitcoinConnector: stored tx R:%s", qPrintable(hash.toHex()));
	}
	if (type == 2) {
		qDebug("cannot use this method to save blocks");
		abort();
	}
}

void ModBitcoinConnector::addBlock(const BitcoinBlock&block) {
	if (!block.isValid()) return;

	quint32 type = 2;
	QByteArray hash = block.getHash();
	QByteArray parent = block.getParent();
	QByteArray raw = block.getRaw(); // only 80 bytes
	QByteArray key;
	QList<BitcoinTx> txs = block.getTransactions();

	for(int i = 0; i < txs.size(); i++) {
		if (!knows(1, txs[i].hash()))
			addInventory(1, txs[i].hash(), txs[i].generate(), false);

		QJsonObject block_tx_obj;
		block_tx_obj.insert("block_tx", QJsonValue(QString(hash.toHex()+txs[i].hash().toHex())));
		block_tx_obj.insert("block", QJsonValue(QString(hash.toHex())));
		block_tx_obj.insert("tx", QJsonValue(QString(txs[i].hash().toHex())));
		block_tx_obj.insert("index", QJsonValue(i));
		db->index(block_tx_obj, "bitcoin", "block_tx");
	}

	QDataStream s(&key, QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << type;
	key.append(hash);

	inventory_cache.insert(key, new QByteArray(block.getRaw()));
	inventory_queue.append(key);

	QJsonObject block_obj;
	block_obj.insert("hash", QString(hash.toHex()));
	block_obj.insert("parent", QString(parent.toHex()));
	block_obj.insert("depth", (qint64)block.getHeight());
	block_obj.insert("version", (qint64)block.getVersion());
	block_obj.insert("mrkl_root", QString(block.getMerkleRoot().toHex()));
	block_obj.insert("time", (qint64)block.getTimestamp());
	block_obj.insert("bits", (qint64)block.getBits());
	block_obj.insert("nonce", (qint64)block.getNonce());
	block_obj.insert("size", (qint64)block.getSize());
	block_obj.insert("raw_header", QJsonValue(QString(raw.toBase64())));
	db->index(block_obj, "bitcoin", "block");
}

void ModBitcoinConnector::sendQueuedInventory() {
	if (inventory_queue.isEmpty()) return;
	QByteArray buf;
	for(qint32 i = 0; i < inventory_queue.length(); i++)
		buf.append(inventory_queue.at(i));
	newInventory(inventory_queue.length(), buf);
	inventory_queue.clear();
}

void ModBitcoinConnector::registerPeer(const BitcoinNetAddr &a, quint32 stamp) {
	registerPeer(a.getKey(), a.getServices(), stamp);
}

void ModBitcoinConnector::registerPeer(const QString &key, quint64 services, quint32 stamp) {
	if (stamp > time(NULL)) stamp = time(NULL);
	qDebug("got peer: %s %llu %u", qPrintable(key), services, stamp);
	if (!peer_learn) return; // do not learn/connect to peer
	// should connect to new peer
}

void ModBitcoinConnector::doDatabaseInit() {
	// create db struct
}

void ModBitcoinConnector::reload() {
	QMap<QString,QVariant> conf = getConfig();

	if (!db_open) {
		db = new ElasticSearch(conf.value("es_host","http://127.0.0.1:9200").toString(), this);

		doDatabaseInit();

		db_open = true;
	}

	peer_learn = conf.value("peer_learn", true).toBool();

	QStringList conf_peers;
	if (conf.contains("peer_seed")) {
		conf_peers = conf.value("peer_seed").toStringList();
	} else {
		// random nodes
		conf_peers << "jun.dashjr.org/8333";
		conf_peers << "w003.mo.us.xta.net/8333";
		conf_peers << "bitcoincharts.com/8333";
	}

	for(int i = 0; i < conf_peers.size(); i++) {
		if (peers.contains(conf_peers.at(i))) continue; // already connected

		qDebug("initiating connection to %s", qPrintable(conf_peers.at(i)));
		QTcpSocket *tmpsock = new QTcpSocket(this);
		tmp_cnx.append(tmpsock);
		connect(tmpsock, SIGNAL(connected()), this, SLOT(outgoingTcp()));

		QStringList tmp = conf_peers.at(i).split("/");
		if (tmp.size() != 2) continue; // meh?
		tmpsock->connectToHost(tmp.at(0), tmp.at(1).toInt());
	}
}

void ModBitcoinConnector::unregisterPeer(const QString&s) {
	peers.remove(s);
}

void ModBitcoinConnector::clientInit(ModBitcoinConnectorClient*n) {
	connect(this, SIGNAL(newInventory(quint32,const QByteArray&)), n, SLOT(newInventory(quint32,const QByteArray&)));
	peers.insert(n->getKey(), n);
}

void ModBitcoinConnector::outgoingTcp() {
	QTcpSocket *s = qobject_cast<QTcpSocket*>(sender());
	if (s == NULL) return;
	ModBitcoinConnectorClient *n = new ModBitcoinConnectorClient(s, this);
	clientInit(n);
	clientAdd(n);
	n->setOutgoing(true);
	n->sendVersion();
}

void ModBitcoinConnector::incomingTcp(const QString &, QTcpSocket *sock) {
	ModBitcoinConnectorClient *n = new ModBitcoinConnectorClient(sock, this);
	clientInit(n);
	clientAdd(n);
}

void ModBitcoinConnector::registerNonce(const QByteArray&n) {
	nonce.insert(n);
}

void ModBitcoinConnector::unregisterNonce(const QByteArray&n) {
	nonce.remove(n);
}

bool ModBitcoinConnector::knownNonce(const QByteArray&n) {
	return nonce.contains(n);
}
