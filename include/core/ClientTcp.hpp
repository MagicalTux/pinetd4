#include "Client.hpp"

class QTcpSocket;

class ClientTcp: public Client {
	Q_OBJECT;
public:
	ClientTcp(QTcpSocket *sock, Daemon *parent);

public slots:
	void doRead();
	void bytesWritten(qint64);

protected:
	QTcpSocket *sock;
	virtual void handleBuffer(const QByteArray&) = 0;
	virtual void write(const QByteArray &dat);
	virtual void flush();

	QByteArray write_buf;
};

