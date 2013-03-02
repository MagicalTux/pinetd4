#include "Client.hpp"

class QTcpSocket;

class ClientTcp: public Client {
	Q_OBJECT;
public:
	ClientTcp(QTcpSocket *sock, Daemon *parent);

public slots:
	void doRead();

protected:
	QTcpSocket *sock;
	virtual void handleBuffer(const QByteArray&) = 0;
};

