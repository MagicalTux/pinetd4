#include <core/Daemon.hpp>

class ModPS: public Daemon {
	Q_OBJECT;
public:
	ModPS(const QString &modname, const QString &instname);

public slots:
	virtual void incomingTcp(const QString &entry, QTcpSocket *sock);
};

