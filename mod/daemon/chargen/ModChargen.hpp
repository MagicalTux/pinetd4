#include <core/Daemon.hpp>

class ModChargen: public Daemon {
	Q_OBJECT;
public:
	ModChargen(const QString &modname, const QString &instname);

public slots:
	virtual void incomingTcp(const QString &entry, QTcpSocket *sock);
};

