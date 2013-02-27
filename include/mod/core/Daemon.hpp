#include <QObject>
#include <QMap>

class QTcpSocket;
class Client;

class Daemon: public QObject {
	Q_OBJECT;
public:
	Daemon(const QString &modname, const QString &instname);

public slots:
	virtual void incomingTcp(const QString &entry, QTcpSocket *sock);

protected:
	void clientAdd(Client *);

	QString modname;
	QString instname;
	QMap <QString, Client*> clients;
};

