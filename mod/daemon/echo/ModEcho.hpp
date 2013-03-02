#include <core/Daemon.hpp>

class ModEcho: public Daemon {
	Q_OBJECT;
public:
	ModEcho(const QString &modname, const QString &instname);

public slots:
	virtual void incomingTcp(const QString &entry, QTcpSocket *sock);
};

