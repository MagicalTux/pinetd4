#include <core/Daemon.hpp>

class ModKumoIRCd: public Daemon {
	Q_OBJECT;
public:
	ModKumoIRCd(const QString &modname, const QString &instname);
	QByteArray getServerName();

public slots:
	virtual void incomingTcp(const QString &entry, QTcpSocket *sock);
};

