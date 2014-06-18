#include <core/Daemon.hpp>

class DatagramReply;

class ModNetflow: public Daemon {
	Q_OBJECT;
public:
	ModNetflow(const QString &modname, const QString &instname);

public slots:
	void incomingDatagram(const QByteArray&, DatagramReply*);

private:
};

