#include <core/Daemon.hpp>

class DatagramReply;

class ModPS: public Daemon {
	Q_OBJECT;
public:
	ModPS(const QString &modname, const QString &instname);

public slots:
	virtual void incomingTcp(const QString &entry, QTcpSocket *sock);
	void incomingDatagram(const QByteArray&, DatagramReply*);
};

