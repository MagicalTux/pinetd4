#include "ModBitcoinConnector.hpp"
#include "ModBitcoinConnectorClient.hpp"
#include <QStringList>
#include <QVariant>
#include <QTcpSocket>
#include <QSqlQuery>
#include <QSqlError>
#include <ext/BitcoinNetAddr.hpp>
#include <ext/BitcoinBlock.hpp>

#if 0
#define SQL_QUERY(_var, _req) static bool req_prepared_ ## _var = false; QSqlQuery _var(db); if (!req_prepared_ ## _var) { req_prepared_ ## _var = true; _var.prepare(_req); _var.setForwardOnly(true); }
#define SQL_BIND(_query, _var, _val) static QVariant req_bind_var_ ## _query ## _var; static bool req_bind_var_ ## _query ## _var ## _bound = false; if (!req_bind_var_ ## _query ## _var ## _bound) { req_bind_var_ ## _query ## _var ## _bound = true; _query.bindValue(":" #_var, req_bind_var_ ## _query ## _var); } req_bind_var_ ## _query ## _var = _val;
#else
#define SQL_QUERY(_var, _req) QSqlQuery _var(db); _var.prepare(_req); _var.setForwardOnly(true);
#define SQL_BIND(_query, _var, _val) _query.bindValue(":" #_var, _val);
#endif

ModBitcoinConnector::ModBitcoinConnector(const QString &modname, const QString &instname): Daemon(modname, instname), db_lock(QMutex::Recursive) {
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
	db_lock.lock();
	SQL_QUERY(query, "SELECT height FROM blocks ORDER BY height DESC LIMIT 1");
	if (!query.exec()) {
		db_lock.unlock();
		return 0; // error
	}
	if (!query.next()) {
		db_lock.unlock();
		return 0; // wat?
	}
	int res = query.value(0).toInt();
	db_lock.unlock();
	return res;
}

BitcoinBlock ModBitcoinConnector::getLastBlock() {
	db_lock.lock();
	SQL_QUERY(query, "SELECT height, data FROM blocks ORDER BY height DESC LIMIT 1");
	if (!query.exec()) {
		db_lock.unlock();
		return BitcoinBlock(); // error
	}
	if (!query.next()) {
		db_lock.unlock();
		return BitcoinBlock(); // wat?
	}
	quint32 height = query.value(0).toUInt();
	QByteArray res = query.value(1).toByteArray();
	db_lock.unlock();
	return BitcoinBlock(res, height);
}

BitcoinBlock ModBitcoinConnector::getBlock(const QByteArray &hash) {
	db_lock.lock();
	SQL_QUERY(query, "SELECT height, data FROM blocks WHERE hash = :hash");
	SQL_BIND(query, hash, hash);
	if (!query.exec()) {
		db_lock.unlock();
		return BitcoinBlock(); // error
	}
	if (!query.next()) {
		db_lock.unlock();
		return BitcoinBlock(); // wat?
	}
	int height = query.value(0).toInt();
	QByteArray res = query.value(1).toByteArray();
	return BitcoinBlock(res, height);
}

QByteArray ModBitcoinConnector::getInventory(quint32 type, const QByteArray &hash) {
	QByteArray key;
	QDataStream s(&key, QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << type;
	key.append(hash);

	if (inventory_cache.contains(key)) return *inventory_cache.object(key);

	if (type == 1) {
		db_lock.lock();
		SQL_QUERY(query, "SELECT data FROM tx WHERE txid = :txid");
		SQL_BIND(query, txid, hash);
		if (!query.exec()) {
			db_lock.unlock();
			return QByteArray(); // error
		}
		if (!query.next()) {
			db_lock.unlock();
			return QByteArray(); // wat?
		}
		QByteArray res = query.value(0).toByteArray();
		inventory_cache.insert(key, new QByteArray(res));
		db_lock.unlock();
		return res;
	}
	if (type == 2) {
		db_lock.lock();
		SQL_QUERY(query, "SELECT data FROM blocks WHERE hash = :hash");
		SQL_BIND(query, hash, hash);
		if (!query.exec()) {
			db_lock.unlock();
			return QByteArray(); // error
		}
		if (!query.next()) {
			db_lock.unlock();
			return QByteArray(); // wat?
		}
		QByteArray res = query.value(0).toByteArray();
		inventory_cache.insert(key, new QByteArray(res));
		db_lock.unlock();
		return res;
	}
	return QByteArray();
}

bool ModBitcoinConnector::knows(quint32 type, const QByteArray &hash) {
	QByteArray key;
	QDataStream s(&key, QIODevice::WriteOnly); s.setByteOrder(QDataStream::LittleEndian); s << type;
	key.append(hash);

//	qDebug("ModBitcoinConnector: do I know %d:%s?", type, qPrintable(hash.toHex()));
	if (inventory_cache.contains(key)) return true;

	if (type == 1) {
		db_lock.lock();
		SQL_QUERY(query, "SELECT COUNT(*) FROM tx WHERE txid = :txid");
		SQL_BIND(query, txid, hash);
		if (!query.exec()) {
			db_lock.unlock();
			return false; // error
		}
		if (!query.next()) {
			db_lock.unlock();
			return false; // wat?
		}
		int res = query.value(0).toInt();
		db_lock.unlock();
		return res > 0;
	}
	if (type == 2) {
		db_lock.lock();
		SQL_QUERY(query, "SELECT COUNT(*) FROM blocks WHERE hash = :hash");
		SQL_BIND(query, hash, hash);
		if (!query.exec()) {
			db_lock.unlock();
			return false; // error
		}
		if (!query.next()) {
			db_lock.unlock();
			return false; // wat?
		}
		int res = query.value(0).toInt();
		db_lock.unlock();
		return res > 0;
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
		db_lock.lock();
		SQL_QUERY(query, "INSERT INTO tx (txid, data) VALUES (:txid, :blob)");
		SQL_BIND(query, txid, hash);
		SQL_BIND(query, blob, data);
		if (!query.exec())
			qDebug("failed to exec query: %s", qPrintable(query.lastError().text()));
		db_lock.unlock();
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

	db_lock.lock();
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
			db_lock.unlock();
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
		db_lock.unlock();
		return;
	}
	db.commit();
	db_lock.unlock();
}

void ModBitcoinConnector::sendQueuedInventory() {
	if (inventory_queue.isEmpty()) return;
	QByteArray buf;
	for(qint32 i = 0; i < inventory_queue.length(); i++)
		buf.append(inventory_queue.at(i));
	newInventory(inventory_queue.length(), buf);
	inventory_queue.clear();
}

void ModBitcoinConnector::doDatabaseInit() {
	db_lock.lock();
	// indexes we need:
	// [blockheight=>block], addr=>tx_in, addr=>tx_out, [parentblock=>block]
	db.exec("PRAGMA synchronous = OFF");

	db.exec("CREATE TABLE IF NOT EXISTS tx (txid CHAR(64), data LONGBLOB)");
	db.exec("CREATE UNIQUE INDEX IF NOT EXISTS tx_txid ON tx(txid)");

	db.exec("CREATE TABLE IF NOT EXISTS blocks (hash CHAR(64), parent CHAR(64), height INT, data LONGBLOB)");
	db.exec("CREATE UNIQUE INDEX IF NOT EXISTS blocks_hash ON blocks(hash)");
	db.exec("CREATE INDEX IF NOT EXISTS blocks_parent ON blocks(parent)");
	db.exec("CREATE INDEX IF NOT EXISTS blocks_height ON blocks(height)");

	db.exec("CREATE TABLE IF NOT EXISTS blocks_txs (block CHAR(64), tx CHAR(64), idx INT)");
	db.exec("CREATE UNIQUE INDEX IF NOT EXISTS blocks_txs_full ON blocks_txs(block,idx)");
	db.exec("CREATE INDEX IF NOT EXISTS blocks_txs_tx ON blocks_txs(tx)");
	db.exec("CREATE INDEX IF NOT EXISTS blocks_txs_block ON blocks_txs(block)");
	db_lock.unlock();
}

void ModBitcoinConnector::reload() {
	QMap<QString,QVariant> conf = getConfig();

	if (!db_open) {
		if (!conf.contains("db_type"))
			conf.insert("db_type", "SQLITE");

		if ((!conf.contains("db_database")) && (conf.value("db_type") == "SQLITE"))
			conf.insert("db_database", "/tmp/bitcoinconnector.db");

		db_lock.lock();
		db = QSqlDatabase::addDatabase(conf.value("db_type").toString(), instanceName());
		db.setHostName(conf.value("db_hostname").toString());
		if (conf.contains("db_username")) db.setUserName(conf.value("db_username").toString());
		if (conf.contains("db_password")) db.setPassword(conf.value("db_password").toString());
		if (conf.contains("db_database")) db.setDatabaseName(conf.value("db_database").toString());
		if (conf.contains("db_options")) db.setConnectOptions(conf.value("db_options").toString());

		if (!db.open()) {
			qDebug("failed to open database: %s", qPrintable(db.lastError().text()));
			qDebug() << conf;
			db_lock.unlock();
			return;
		}
		db_lock.unlock();

		doDatabaseInit();

		db_open = true;
	}

	QStringList conf_peers;
	if (conf.contains("peer_seed")) {
		conf_peers = conf.value("peer_seed").toStringList();
	} else {
		// random nodes
		conf_peers << "jun.dashjr.org:8333";
//		conf_peers << "w003.mo.us.xta.net:8333";
//		conf_peers << "bitcoincharts.com:8333";
	}

	for(int i = 0; i < conf_peers.size(); i++) {
		if (peers.contains(conf_peers.at(i))) continue; // already connected

		qDebug("initiating connection to %s", qPrintable(conf_peers.at(i)));
		QTcpSocket *tmpsock = new QTcpSocket(this);
		tmp_cnx.append(tmpsock);
		connect(tmpsock, SIGNAL(connected()), this, SLOT(outgoingTcp()));

		QStringList tmp = conf_peers.at(i).split(":");
		if (tmp.size() != 2) continue; // meh?
		tmpsock->connectToHost(tmp.at(0), tmp.at(1).toInt());
	}
}

void ModBitcoinConnector::clientInit(ModBitcoinConnectorClient*n) {
	connect(this, SIGNAL(newInventory(quint32,const QByteArray&)), n, SLOT(newInventory(quint32,const QByteArray&)));
}

void ModBitcoinConnector::outgoingTcp() {
	QTcpSocket *s = qobject_cast<QTcpSocket*>(sender());
	if (s == NULL) return;
	ModBitcoinConnectorClient *n = new ModBitcoinConnectorClient(s, this);
	clientInit(n);
	clientAdd(n);
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
