#include <core/Daemon.hpp>

class ModRConnect: public Daemon {
	Q_OBJECT;
public:
	ModRConnect(const QString &modname, const QString &instname);

public slots:
	virtual void incomingTcp(const QString &entry, QTcpSocket *sock);
};

