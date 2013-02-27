#include "Client.hpp"

class QTcpSocket;

class ClientTcp: public Client {
	Q_OBJECT;
public:
	ClientTcp(QTcpSocket *sock, Daemon *parent);

protected:
	QTcpSocket *sock;
};

