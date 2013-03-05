#include <core/ClientTcp.hpp>

class ClientTcpLine: public ClientTcp {
	Q_OBJECT;
public:
	ClientTcpLine(QTcpSocket *sock, Daemon *parent);
	// TODO

protected:
	virtual void handleLine(const QByteArray&) = 0; // handle one line of input
	virtual void handleBuffer(const QByteArray&); // read buffer, parse it
	QByteArray read_buffer;
};

