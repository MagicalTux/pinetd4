#include <core/Daemon.hpp>

class ModVServ: public Daemon {
	Q_OBJECT;
public:
	ModVServ(const QString &modname, const QString &instname);

public slots:
	virtual void incomingTcp(const QString &entry, QTcpSocket *sock);
};

