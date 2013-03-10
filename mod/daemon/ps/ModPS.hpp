#include <core/Daemon.hpp>
#include <QMutex>
#include <QTimer>

class DatagramReply;

class ModPS: public Daemon {
	Q_OBJECT;
public:
	ModPS(const QString &modname, const QString &instname);

	void channelAddRef(const QByteArray &channel);
	void channelDelRef(const QByteArray &channel);

signals:
	void channelPacket(const QByteArray &dat, const QByteArray &chan, int type);

public slots:
	virtual void incomingTcp(const QString &entry, QTcpSocket *sock);
	virtual void reload();
	void incomingDatagram(const QByteArray&, DatagramReply*);
	void checkMasters();
	void readFromMaster();
	void masterConnected();

private:
	void pushPacket(const QByteArray&);
	void doSubscribe(const QByteArray&);
	void doUnsubscribe(const QByteArray&);

	QMap<QByteArray,int> channel_refcount;
	QMutex channel_refcount_lock;
	QMap<QString,QTcpSocket*> masters;
	QTimer masters_check;
};

