#include "ModBitcoinConnector.hpp"
#include "ModBitcoinConnectorClient.hpp"
#include <QStringList>
#include <QVariant>
#include <QTcpSocket>
#include <ext/BitcoinNetAddr.hpp>
#include <ext/BitcoinBlock.hpp>

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

	// make things [faster/easier to read] by passing json string directly
	QJsonObject stat = db->search("{\"index\":\"bitcoin\",\"body\":{\"query\":{\"match_all\":{}},\"facets\":{\"stat1\":{\"statistical\":{\"field\":\"block.depth\"}}}}}");

	return stat.value("facets").toObject().value("stat1").toObject().value("max").toDouble();
}

BitcoinBlock ModBitcoinConnector::getLastBlock() {
#if 0
	SQL_QUERY(query, "SELECT height, data FROM blocks ORDER BY height DESC LIMIT 1");
	if (!query.exec()) {
		return BitcoinBlock(); // error
	}
	if (!query.next()) {
		return BitcoinBlock(); // wat?
	}
	quint32 height = query.value(0).toUInt();
	QByteArray res = query.value(1).toByteArray();
	return BitcoinBlock(res, height);
#endif
	return BitcoinBlock();
}

BitcoinBlock ModBitcoinConnector::getBlock(const QByteArray &hash) {
#if 0
	SQL_QUERY(query, "SELECT height, data FROM blocks WHERE hash = :hash");
	SQL_BIND(query, hash, hash);
	if (!query.exec()) {
		return BitcoinBlock(); // error
	}
	if (!query.next()) {
		return BitcoinBlock(); // wat?
	}
	int height = query.value(0).toInt();
	QByteArray res = query.value(1).toByteArray();
	return BitcoinBlock(res, height);
#endif
	return BitcoinBlock();
}

QByteArray ModBitcoinConnector::getInventory(quint32 type, const QByteArray &hash) {
	QByteArray key;
	QDataStream s(&key, QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << type;
	key.append(hash);

	if (inventory_cache.contains(key)) return *inventory_cache.object(key);

#if 0
	if (type == 1) {
		SQL_QUERY(query, "SELECT data FROM tx WHERE txid = :txid");
		SQL_BIND(query, txid, hash);
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

#if 0
	if (type == 1) {
		SQL_QUERY(query, "SELECT COUNT(*) FROM tx WHERE txid = :txid");
		SQL_BIND(query, txid, hash);
		if (!query.exec()) {
			return false; // error
		}
		if (!query.next()) {
			return false; // wat?
		}
		int res = query.value(0).toInt();
		return res > 0;
	}
	if (type == 2) {
		SQL_QUERY(query, "SELECT COUNT(*) FROM blocks WHERE hash = :hash");
		SQL_BIND(query, hash, hash);
		if (!query.exec()) {
			return false; // error
		}
		if (!query.next()) {
			return false; // wat?
		}
		int res = query.value(0).toInt();
		return res > 0;
	}
#endif
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

#if 0
	if (type == 1) {
		SQL_QUERY(query, "INSERT INTO tx (txid, data) VALUES (:txid, :blob)");
		SQL_BIND(query, txid, hash);
		SQL_BIND(query, blob, data);
		if (!query.exec())
			qDebug("failed to exec query: %s", qPrintable(query.lastError().text()));
	}
	if (type == 2) {
		qDebug("cannot use this method to save blocks");
		abort();
	}
#endif
}

void ModBitcoinConnector::addBlock(const BitcoinBlock&block) {
	if (!block.isValid()) return;

	quint32 type = 2;
	QByteArray hash = block.getHash();
	QByteArray parent = block.getParent();
	QByteArray raw = block.getRaw(); // only 80 bytes
	QByteArray key;
	QList<BitcoinTx> txs = block.getTransactions();

#if 0
	db.transaction();

	for(int i = 0; i < txs.size(); i++) {
		if (!knows(1, txs[i].hash()))
			addInventory(1, txs[i].hash(), txs[i].generate(), false);

		SQL_QUERY(query2, "INSERT INTO blocks_txs (block, tx, idx) VALUES (?,?,?)");
		query2.addBindValue(hash);
		query2.addBindValue(txs[i].hash());
		query2.addBindValue(i);
		if (!query2.exec()) {
			qDebug("failed to exec blocks_txs query: %s", qPrintable(query2.lastError().text()));
			db.rollback();
			return;
		}
	}

	QDataStream s(&key, QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << type;
	key.append(hash);

	inventory_cache.insert(key, new QByteArray(block.getRaw()));
	inventory_queue.append(key);

	SQL_QUERY(query, "INSERT INTO blocks (hash, parent, height, data) VALUES (:hash, :parent, :height, :blob)");
	SQL_BIND(query, hash, hash);
	SQL_BIND(query, parent, parent);
	SQL_BIND(query, height, block.getHeight());
	SQL_BIND(query, blob, raw);
	if (!query.exec()) {
		qDebug("failed to exec query: %s", qPrintable(query.lastError().text()));
		db.rollback();
		return;
	}
	db.commit();
#endif
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
	qDebug("got peer: %s %lu %u", qPrintable(key), services, stamp);
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
