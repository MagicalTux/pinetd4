#include <core/Daemon.hpp>
#include <QSet>
#include <QSqlDatabase>
#include <QHostAddress>
#include <QCache>
#include <QTimer>
#include <QMutex>

class QTcpSocket;
class ModBitcoinConnectorClient;
class BitcoinBlock;
class BitcoinNetAddr;

class ModBitcoinConnector: public Daemon {
	Q_OBJECT;
public:
	ModBitcoinConnector(const QString &modname, const QString &instname);
	const QByteArray &getClientId() const;
	quint32 getBlockHeight();

	void registerNonce(const QByteArray&);
	void unregisterNonce(const QByteArray&);
	void unregisterPeer(const QString&);
	bool knownNonce(const QByteArray&);

	bool knows(quint32 type, const QByteArray &hash);
	void addInventory(quint32 type, const QByteArray &hash, const QByteArray &data, bool send_inv = true);
	void addBlock(const BitcoinBlock&);
	QByteArray getInventory(quint32 type, const QByteArray &hash);

	BitcoinBlock getBlock(const QByteArray &hash);
	BitcoinBlock getLastBlock();

	void registerPeer(const BitcoinNetAddr &, quint32 stamp);
	void registerPeer(const QString &key, quint64 services, quint32 stamp);

signals:
	void newInventory(quint32 count, const QByteArray &);

public slots:
	virtual void incomingTcp(const QString &entry, QTcpSocket *sock);
	void outgoingTcp();
	void reload();
	void sendQueuedInventory();

private:
	void clientInit(ModBitcoinConnectorClient*);
	void doDatabaseInit();

	QList<QTcpSocket*> tmp_cnx;
	QMap<QString,ModBitcoinConnectorClient*> peers;
	QByteArray client_id;
	QSet<QByteArray> nonce;

	bool db_open;
	QSqlDatabase db;
	QMutex db_lock;
	QCache<QByteArray, QByteArray> inventory_cache;

	QList<QByteArray> inventory_queue;
	QTimer inventory_queue_timer;
};

