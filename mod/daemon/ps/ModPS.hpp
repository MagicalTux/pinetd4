#include <core/Daemon.hpp>
#include <QMutex>

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
	void incomingDatagram(const QByteArray&, DatagramReply*);

private:
	void pushPacket(const QByteArray&);
	QMap<QByteArray,int> channel_refcount;
	QMutex channel_refcount_lock;
};

