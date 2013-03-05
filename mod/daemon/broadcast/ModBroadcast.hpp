#include <core/Daemon.hpp>

class ModBroadcast: public Daemon {
	Q_OBJECT;
public:
	ModBroadcast(const QString &modname, const QString &instname);

signals:
	void broadcast(const QByteArray&);

public slots:
	virtual void incomingTcp(const QString &entry, QTcpSocket *sock);
	void incoming(const QByteArray&);
};

